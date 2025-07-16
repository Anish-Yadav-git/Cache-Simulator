#ifndef CACHE_GUI_H
#define CACHE_GUI_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QSplitter>
#include <QProgressBar>
#include <QStatusBar>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QMessageBox>
#include <QApplication>
#include <QVBoxLayout>
#include <memory>

class CacheGUI : public QMainWindow
{
    Q_OBJECT

public:
    CacheGUI(QWidget *parent = nullptr);
    ~CacheGUI();

private slots:
    void createCache();
    void resetCache();
    void accessCache();
    void batchAccess();
    void updateCacheVisualization();
    void clearLog();
    void exportLog();
    void onNetworkReply();

private:
    void setupUI();
    void setupConfigurationPanel();
    void setupMemoryAccessPanel();
    void setupCacheVisualizationPanel();
    void setupStatisticsPanel();
    void setupLogPanel();
    void setupStatusBar();
    
    void updateStatistics(const QJsonObject& stats);
    void logAccess(const QString& address, const QString& operation, const QString& result);
    void showMessage(const QString& message, const QString& type = "info");
    void makeAPICall(const QString& endpoint, const QJsonObject& params = QJsonObject());
    
    // Configuration controls
    QComboBox* m_cacheSizeCombo;
    QComboBox* m_blockSizeCombo;
    QComboBox* m_associativityCombo;
    QComboBox* m_replacementPolicyCombo;
    QComboBox* m_writePolicyCombo;
    QComboBox* m_writeMissPolicyCombo;
    QPushButton* m_createCacheBtn;
    QPushButton* m_resetCacheBtn;
    
    // Memory access controls
    QLineEdit* m_addressEdit;
    QComboBox* m_operationCombo;
    QPushButton* m_singleAccessBtn;
    QPushButton* m_batchAccessBtn;
    QTextEdit* m_batchAddressEdit;
    QPushButton* m_runBatchBtn;
    
    // Cache visualization
    QTableWidget* m_cacheTable;
    QLabel* m_cacheInfoLabel;
    
    // Statistics
    QLabel* m_totalAccessesLabel;
    QLabel* m_hitsLabel;
    QLabel* m_missesLabel;
    QLabel* m_hitRateLabel;
    QLabel* m_missRateLabel;
    
    // Log
    QTextEdit* m_logTextEdit;
    QPushButton* m_clearLogBtn;
    QPushButton* m_exportLogBtn;
    
    // Network
    QNetworkAccessManager* m_networkManager;
    QString m_baseURL;
    
    // Status
    QStatusBar* m_statusBar;
    QProgressBar* m_progressBar;
    
    // Cache state
    bool m_cacheCreated;
    int m_cacheSize;
    int m_blockSize;
    int m_associativity;
    int m_numSets;
    
    // UI Groups
    QGroupBox* m_configGroup;
    QGroupBox* m_accessGroup;
    QGroupBox* m_vizGroup;
    QGroupBox* m_statsGroup;
    QGroupBox* m_logGroup;
};

#endif // CACHE_GUI_H
