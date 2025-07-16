#include "cache_gui.h"
#include <QUrl>
#include <QUrlQuery>
#include <QFileDialog>
#include <QTextStream>
#include <QTimer>
#include <QSplitter>
#include <QGroupBox>
#include <QGridLayout>
#include <QHeaderView>
#include <QTableWidgetItem>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QTime>
#include <QFile>
#include <QIODevice>

CacheGUI::CacheGUI(QWidget *parent)
    : QMainWindow(parent),
      m_networkManager(new QNetworkAccessManager(this)),
      m_baseURL("http://localhost:8080"),
      m_cacheCreated(false)
{
    setWindowTitle("Cache Simulator GUI");
    setMinimumSize(1200, 800);
    
    setupUI();
    setupStatusBar();
    
    // Connect network manager
    connect(m_networkManager, &QNetworkAccessManager::finished, this, &CacheGUI::onNetworkReply);
}

CacheGUI::~CacheGUI()
{
}

void CacheGUI::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    
    // Create splitter for main layout
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Left panel for configuration and access
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    setupConfigurationPanel();
    setupMemoryAccessPanel();
    
    leftLayout->addWidget(m_configGroup);
    leftLayout->addWidget(m_accessGroup);
    leftLayout->addStretch();
    
    // Right panel for visualization and statistics
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    setupCacheVisualizationPanel();
    setupStatisticsPanel();
    setupLogPanel();
    
    rightLayout->addWidget(m_vizGroup);
    rightLayout->addWidget(m_statsGroup);
    rightLayout->addWidget(m_logGroup);
    
    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setSizes({400, 800});
    
    mainLayout->addWidget(mainSplitter);
    setCentralWidget(centralWidget);
}

void CacheGUI::setupConfigurationPanel()
{
    m_configGroup = new QGroupBox("Cache Configuration");
    QGridLayout *layout = new QGridLayout(m_configGroup);
    
    // Cache size
    layout->addWidget(new QLabel("Cache Size:"), 0, 0);
    m_cacheSizeCombo = new QComboBox();
    m_cacheSizeCombo->addItems({"256", "512", "1024", "2048", "4096"});
    m_cacheSizeCombo->setCurrentText("1024");
    layout->addWidget(m_cacheSizeCombo, 0, 1);
    
    // Block size
    layout->addWidget(new QLabel("Block Size:"), 1, 0);
    m_blockSizeCombo = new QComboBox();
    m_blockSizeCombo->addItems({"16", "32", "64"});
    m_blockSizeCombo->setCurrentText("32");
    layout->addWidget(m_blockSizeCombo, 1, 1);
    
    // Associativity
    layout->addWidget(new QLabel("Associativity:"), 2, 0);
    m_associativityCombo = new QComboBox();
    m_associativityCombo->addItems({"1", "2", "4", "8", "0"});
    m_associativityCombo->setCurrentText("4");
    layout->addWidget(m_associativityCombo, 2, 1);
    
    // Replacement policy
    layout->addWidget(new QLabel("Replacement Policy:"), 3, 0);
    m_replacementPolicyCombo = new QComboBox();
    m_replacementPolicyCombo->addItems({"LRU", "FIFO", "RANDOM"});
    layout->addWidget(m_replacementPolicyCombo, 3, 1);
    
    // Write policy
    layout->addWidget(new QLabel("Write Policy:"), 4, 0);
    m_writePolicyCombo = new QComboBox();
    m_writePolicyCombo->addItems({"WRITE_THROUGH", "WRITE_BACK"});
    layout->addWidget(m_writePolicyCombo, 4, 1);
    
    // Write miss policy
    layout->addWidget(new QLabel("Write Miss Policy:"), 5, 0);
    m_writeMissPolicyCombo = new QComboBox();
    m_writeMissPolicyCombo->addItems({"WRITE_ALLOCATE", "NO_WRITE_ALLOCATE"});
    layout->addWidget(m_writeMissPolicyCombo, 5, 1);
    
    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_createCacheBtn = new QPushButton("Create Cache");
    m_resetCacheBtn = new QPushButton("Reset Cache");
    m_resetCacheBtn->setEnabled(false);
    
    buttonLayout->addWidget(m_createCacheBtn);
    buttonLayout->addWidget(m_resetCacheBtn);
    
    layout->addLayout(buttonLayout, 6, 0, 1, 2);
    
    // Connect signals
    connect(m_createCacheBtn, &QPushButton::clicked, this, &CacheGUI::createCache);
    connect(m_resetCacheBtn, &QPushButton::clicked, this, &CacheGUI::resetCache);
}

void CacheGUI::setupMemoryAccessPanel()
{
    m_accessGroup = new QGroupBox("Memory Access");
    QVBoxLayout *layout = new QVBoxLayout(m_accessGroup);
    
    // Single access
    QHBoxLayout *singleLayout = new QHBoxLayout();
    singleLayout->addWidget(new QLabel("Address:"));
    m_addressEdit = new QLineEdit();
    m_addressEdit->setPlaceholderText("0x1000");
    
    // Set up hex validator
    QRegularExpression hexRegex("^0x[0-9A-Fa-f]+$");
    QRegularExpressionValidator *hexValidator = new QRegularExpressionValidator(hexRegex, this);
    m_addressEdit->setValidator(hexValidator);
    
    singleLayout->addWidget(m_addressEdit);
    
    m_operationCombo = new QComboBox();
    m_operationCombo->addItems({"READ", "WRITE"});
    singleLayout->addWidget(m_operationCombo);
    
    m_singleAccessBtn = new QPushButton("Access");
    m_singleAccessBtn->setEnabled(false);
    singleLayout->addWidget(m_singleAccessBtn);
    
    layout->addLayout(singleLayout);
    
    // Batch access
    m_batchAccessBtn = new QPushButton("Batch Access");
    m_batchAccessBtn->setEnabled(false);
    layout->addWidget(m_batchAccessBtn);
    
    m_batchAddressEdit = new QTextEdit();
    m_batchAddressEdit->setPlaceholderText("Enter addresses (one per line):\n0x0\n0x20\n0x40\n0x100");
    m_batchAddressEdit->setMaximumHeight(100);
    m_batchAddressEdit->setVisible(false);
    layout->addWidget(m_batchAddressEdit);
    
    m_runBatchBtn = new QPushButton("Run Batch");
    m_runBatchBtn->setVisible(false);
    layout->addWidget(m_runBatchBtn);
    
    // Connect signals
    connect(m_singleAccessBtn, &QPushButton::clicked, this, &CacheGUI::accessCache);
    connect(m_batchAccessBtn, &QPushButton::clicked, this, &CacheGUI::batchAccess);
    connect(m_runBatchBtn, &QPushButton::clicked, this, [this]() {
        QString addresses = m_batchAddressEdit->toPlainText();
        QStringList addressList = addresses.split('\n', Qt::SkipEmptyParts);
        
        for (const QString& addr : addressList) {
            if (!addr.trimmed().isEmpty()) {
                m_addressEdit->setText(addr.trimmed());
                QTimer::singleShot(100, this, &CacheGUI::accessCache);
            }
        }
    });
}

void CacheGUI::setupCacheVisualizationPanel()
{
    m_vizGroup = new QGroupBox("Cache Visualization");
    QVBoxLayout *layout = new QVBoxLayout(m_vizGroup);
    
    m_cacheInfoLabel = new QLabel("Create a cache to see visualization");
    layout->addWidget(m_cacheInfoLabel);
    
    m_cacheTable = new QTableWidget();
    m_cacheTable->setAlternatingRowColors(true);
    m_cacheTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_cacheTable->horizontalHeader()->setStretchLastSection(true);
    layout->addWidget(m_cacheTable);
    
    QPushButton *refreshBtn = new QPushButton("Refresh Cache View");
    connect(refreshBtn, &QPushButton::clicked, this, &CacheGUI::updateCacheVisualization);
    layout->addWidget(refreshBtn);
}

void CacheGUI::setupStatisticsPanel()
{
    m_statsGroup = new QGroupBox("Statistics");
    QGridLayout *layout = new QGridLayout(m_statsGroup);
    
    layout->addWidget(new QLabel("Total Accesses:"), 0, 0);
    m_totalAccessesLabel = new QLabel("0");
    layout->addWidget(m_totalAccessesLabel, 0, 1);
    
    layout->addWidget(new QLabel("Hits:"), 1, 0);
    m_hitsLabel = new QLabel("0");
    layout->addWidget(m_hitsLabel, 1, 1);
    
    layout->addWidget(new QLabel("Misses:"), 2, 0);
    m_missesLabel = new QLabel("0");
    layout->addWidget(m_missesLabel, 2, 1);
    
    layout->addWidget(new QLabel("Hit Rate:"), 3, 0);
    m_hitRateLabel = new QLabel("0.00%");
    layout->addWidget(m_hitRateLabel, 3, 1);
    
    layout->addWidget(new QLabel("Miss Rate:"), 4, 0);
    m_missRateLabel = new QLabel("0.00%");
    layout->addWidget(m_missRateLabel, 4, 1);
}

void CacheGUI::setupLogPanel()
{
    m_logGroup = new QGroupBox("Access Log");
    QVBoxLayout *layout = new QVBoxLayout(m_logGroup);
    
    m_logTextEdit = new QTextEdit();
    m_logTextEdit->setMaximumHeight(150);
    m_logTextEdit->setReadOnly(true);
    layout->addWidget(m_logTextEdit);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_clearLogBtn = new QPushButton("Clear Log");
    m_exportLogBtn = new QPushButton("Export Log");
    
    buttonLayout->addWidget(m_clearLogBtn);
    buttonLayout->addWidget(m_exportLogBtn);
    buttonLayout->addStretch();
    
    layout->addLayout(buttonLayout);
    
    // Connect signals
    connect(m_clearLogBtn, &QPushButton::clicked, this, &CacheGUI::clearLog);
    connect(m_exportLogBtn, &QPushButton::clicked, this, &CacheGUI::exportLog);
}

void CacheGUI::setupStatusBar()
{
    m_statusBar = statusBar();
    m_progressBar = new QProgressBar();
    m_progressBar->setVisible(false);
    m_statusBar->addPermanentWidget(m_progressBar);
    m_statusBar->showMessage("Ready");
}

void CacheGUI::createCache()
{
    QJsonObject params;
    params["cache_size"] = m_cacheSizeCombo->currentText().toInt();
    params["block_size"] = m_blockSizeCombo->currentText().toInt();
    params["associativity"] = m_associativityCombo->currentText().toInt();
    params["replacement_policy"] = m_replacementPolicyCombo->currentText();
    params["write_policy"] = m_writePolicyCombo->currentText();
    params["write_miss_policy"] = m_writeMissPolicyCombo->currentText();
    
    m_cacheSize = params["cache_size"].toInt();
    m_blockSize = params["block_size"].toInt();
    m_associativity = params["associativity"].toInt();
    m_numSets = (m_associativity == 0) ? 1 : m_cacheSize / (m_blockSize * m_associativity);
    
    makeAPICall("/create", params);
}

void CacheGUI::resetCache()
{
    makeAPICall("/reset");
}

void CacheGUI::accessCache()
{
    if (!m_cacheCreated) {
        showMessage("Please create a cache first", "error");
        return;
    }
    
    QString address = m_addressEdit->text().trimmed();
    if (address.isEmpty()) {
        showMessage("Please enter a memory address", "error");
        return;
    }
    
    QJsonObject params;
    params["address"] = address;
    params["operation"] = m_operationCombo->currentText();
    
    makeAPICall("/access", params);
}

void CacheGUI::batchAccess()
{
    bool visible = m_batchAddressEdit->isVisible();
    m_batchAddressEdit->setVisible(!visible);
    m_runBatchBtn->setVisible(!visible);
    m_batchAccessBtn->setText(visible ? "Batch Access" : "Hide Batch");
}

void CacheGUI::updateCacheVisualization()
{
    if (!m_cacheCreated) {
        return;
    }
    makeAPICall("/contents");
}

void CacheGUI::clearLog()
{
    m_logTextEdit->clear();
}

void CacheGUI::exportLog()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Export Log", "cache_log.txt", "Text Files (*.txt)");
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            stream << m_logTextEdit->toPlainText();
            showMessage("Log exported successfully", "success");
        } else {
            showMessage("Failed to export log", "error");
        }
    }
}

void CacheGUI::updateStatistics(const QJsonObject& stats)
{
    m_totalAccessesLabel->setText(QString::number(stats["total_accesses"].toInt()));
    m_hitsLabel->setText(QString::number(stats["hits"].toInt()));
    m_missesLabel->setText(QString::number(stats["misses"].toInt()));
    m_hitRateLabel->setText(QString::number(stats["hit_rate"].toDouble(), 'f', 2) + "%");
    m_missRateLabel->setText(QString::number(100.0 - stats["hit_rate"].toDouble(), 'f', 2) + "%");
}

void CacheGUI::logAccess(const QString& address, const QString& operation, const QString& result)
{
    QString logEntry = QString("[%1] %2 %3 - %4")
                       .arg(QTime::currentTime().toString())
                       .arg(operation)
                       .arg(address)
                       .arg(result);
    
    m_logTextEdit->append(logEntry);
    m_logTextEdit->ensureCursorVisible();
}

void CacheGUI::showMessage(const QString& message, const QString& type)
{
    if (type == "error") {
        QMessageBox::warning(this, "Error", message);
    } else if (type == "success") {
        QMessageBox::information(this, "Success", message);
    }
    m_statusBar->showMessage(message, 3000);
}

void CacheGUI::makeAPICall(const QString& endpoint, const QJsonObject& params)
{
    QUrl url(m_baseURL + endpoint);
    
    if (!params.isEmpty()) {
        QUrlQuery query;
        for (auto it = params.begin(); it != params.end(); ++it) {
            query.addQueryItem(it.key(), it.value().toVariant().toString());
        }
        url.setQuery(query);
    }
    
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    
    m_progressBar->setVisible(true);
    m_statusBar->showMessage("Processing request...");
    
    m_networkManager->get(request);
}

void CacheGUI::onNetworkReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;
    
    m_progressBar->setVisible(false);
    
    if (reply->error() != QNetworkReply::NoError) {
        showMessage("Network error: " + reply->errorString(), "error");
        reply->deleteLater();
        return;
    }
    
    QByteArray data = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject response = doc.object();
    
    QString endpoint = reply->url().path();
    
    if (response["status"].toString() == "success") {
        if (endpoint == "/create") {
            m_cacheCreated = true;
            m_createCacheBtn->setEnabled(false);
            m_resetCacheBtn->setEnabled(true);
            m_singleAccessBtn->setEnabled(true);
            m_batchAccessBtn->setEnabled(true);
            
            QString info = QString("Cache Created - Size: %1B, Block: %2B, Associativity: %3, Sets: %4")
                          .arg(m_cacheSize).arg(m_blockSize).arg(m_associativity).arg(m_numSets);
            m_cacheInfoLabel->setText(info);
            
            // Initialize cache table
            m_cacheTable->setRowCount(m_numSets);
            m_cacheTable->setColumnCount(m_associativity + 1);
            
            QStringList headers;
            headers << "Set";
            for (int i = 0; i < m_associativity; ++i) {
                headers << QString("Way %1").arg(i);
            }
            m_cacheTable->setHorizontalHeaderLabels(headers);
            
            // Fill with initial data
            for (int row = 0; row < m_numSets; ++row) {
                m_cacheTable->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
                for (int col = 1; col <= m_associativity; ++col) {
                    m_cacheTable->setItem(row, col, new QTableWidgetItem("Invalid"));
                }
            }
            
            updateCacheVisualization();
            showMessage("Cache created successfully", "success");
            
        } else if (endpoint == "/reset") {
            // Reset statistics
            QJsonObject emptyStats;
            emptyStats["total_accesses"] = 0;
            emptyStats["hits"] = 0;
            emptyStats["misses"] = 0;
            emptyStats["hit_rate"] = 0.0;
            updateStatistics(emptyStats);
            
            updateCacheVisualization();
            clearLog();
            showMessage("Cache reset successfully", "success");
            
        } else if (endpoint == "/access") {
            QString result = response["result"].toString();
            QString address = m_addressEdit->text();
            QString operation = m_operationCombo->currentText();
            
            logAccess(address, operation, result);
            
            if (response.contains("statistics")) {
                updateStatistics(response["statistics"].toObject());
            }
            
            updateCacheVisualization();
            
        } else if (endpoint == "/contents") {
            QJsonObject contents = response["contents"].toObject();
            QJsonArray sets = contents["sets"].toArray();
            
            for (int i = 0; i < sets.size() && i < m_cacheTable->rowCount(); ++i) {
                QJsonObject set = sets[i].toObject();
                QJsonArray blocks = set["blocks"].toArray();
                
                for (int j = 0; j < blocks.size() && j < m_associativity; ++j) {
                    QJsonObject block = blocks[j].toObject();
                    QString blockText;
                    
                    if (block["valid"].toBool()) {
                        blockText = QString("V:%1 D:%2 Tag:%3")
                                   .arg(block["valid"].toBool() ? "1" : "0")
                                   .arg(block["dirty"].toBool() ? "1" : "0")
                                   .arg(block["tag"].toString());
                    } else {
                        blockText = "Invalid";
                    }
                    
                    QTableWidgetItem *item = m_cacheTable->item(i, j + 1);
                    if (item) {
                        item->setText(blockText);
                        
                        // Color coding
                        if (block["valid"].toBool()) {
                            if (block["dirty"].toBool()) {
                                item->setBackground(QColor(255, 200, 200)); // Light red for dirty
                            } else {
                                item->setBackground(QColor(200, 255, 200)); // Light green for clean
                            }
                        } else {
                            item->setBackground(QColor(240, 240, 240)); // Light gray for invalid
                        }
                    }
                }
            }
        }
    } else {
        showMessage("Error: " + response["message"].toString(), "error");
    }
    
    reply->deleteLater();
}
