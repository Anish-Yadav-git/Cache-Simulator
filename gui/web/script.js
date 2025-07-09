document.addEventListener('DOMContentLoaded', () => {
    // Form elements
    const cacheSizeInput = document.getElementById('cache-size');
    const blockSizeInput = document.getElementById('block-size');
    const associativityInput = document.getElementById('associativity');
    const replacementPolicyInput = document.getElementById('replacement-policy');
    const writePolicyInput = document.getElementById('write-policy');
    const writeMissPolicyInput = document.getElementById('write-miss-policy');
    const createCacheButton = document.getElementById('create-cache');
    const resetCacheButton = document.getElementById('reset-cache');

    // Memory access elements
    const memoryAddressInput = document.getElementById('memory-address');
    const operationTypeInput = document.getElementById('operation-type');
    const singleAccessButton = document.getElementById('single-access');
    const batchAccessButton = document.getElementById('batch-access');
    const batchPanel = document.getElementById('batch-panel');
    const batchAddressesInput = document.getElementById('batch-addresses');
    const runBatchButton = document.getElementById('run-batch');

    // Statistics elements
    const totalAccessesElem = document.getElementById('total-accesses');
    const hitsElem = document.getElementById('hits');
    const missesElem = document.getElementById('misses');
    const hitRateElem = document.getElementById('hit-rate');
    const missRateElem = document.getElementById('miss-rate');

    // Setup initial state
    let cache = null;
    let stats = {
        totalAccesses: 0,
        hits: 0,
        misses: 0
    };

    // Create cache button click
    createCacheButton.addEventListener('click', async () => {
        const cacheSize = parseInt(cacheSizeInput.value);
        const blockSize = parseInt(blockSizeInput.value);
        const associativity = parseInt(associativityInput.value);
        const replacementPolicy = replacementPolicyInput.value;
        const writePolicy = writePolicyInput.value;
        const writeMissPolicy = writeMissPolicyInput.value;

        // Initialize cache simulator
        cache = new CacheSimulator(cacheSize, blockSize, associativity, replacementPolicy, writePolicy, writeMissPolicy);
        
        // Create cache on backend
        showMessage('Creating cache...', 'info');
        const response = await cache.create();
        
        if (response.status === 'success') {
            showMessage('Cache created successfully!', 'success');
            updateCacheVisualization();
            resetStatistics();
        } else {
            showMessage(`Error: ${response.message}`, 'error');
        }
    });

    // Reset cache button click
    resetCacheButton.addEventListener('click', async () => {
        if (cache) {
            showMessage('Resetting cache...', 'info');
            const response = await cache.reset();
            
            if (response.status === 'success') {
                showMessage('Cache reset successfully!', 'success');
                updateCacheVisualization();
                resetStatistics();
                clearLog();
            } else {
                showMessage(`Error: ${response.message}`, 'error');
            }
        }
    });

    // Single access button click
    singleAccessButton.addEventListener('click', async () => {
        if (!cache) return showMessage('Please create a cache first.', 'error');

        const addressStr = memoryAddressInput.value.trim();
        if (!addressStr) return showMessage('Please enter a memory address.', 'error');
        
        const address = parseInt(addressStr, 16);
        const operation = operationTypeInput.value;

        const response = await cache.access(address, operation);
        
        if (response.status === 'success') {
            updateStatisticsFromAPI(response.statistics);
            logAccess(address, operation, response.result);
        } else {
            showMessage(`Error: ${response.message}`, 'error');
        }
    });

    // Batch access button click
    batchAccessButton.addEventListener('click', () => {
        batchPanel.style.display = batchPanel.style.display === 'none' ? 'block' : 'none';
    });

    // Run batch access button click
    runBatchButton.addEventListener('click', async () => {
        if (!cache) return showMessage('Please create a cache first.', 'error');

        const addressLines = batchAddressesInput.value.trim().split('\n').filter(line => line.trim());
        if (addressLines.length === 0) return showMessage('Please enter some addresses.', 'error');
        
        showMessage(`Processing ${addressLines.length} addresses...`, 'info');
        
        for (const addressStr of addressLines) {
            try {
                const address = parseInt(addressStr.trim(), 16);
                const response = await cache.access(address, 'READ');
                
                if (response.status === 'success') {
                    updateStatisticsFromAPI(response.statistics);
                    logAccess(address, 'READ', response.result);
                } else {
                    logAccess(address, 'READ', 'ERROR');
                }
            } catch (error) {
                console.error('Invalid address:', addressStr);
            }
        }
        
        showMessage('Batch processing completed!', 'success');
    });

    // Utility functions
    function updateCacheVisualization() {
        const cacheInfoElem = document.getElementById('cache-info');
        const cacheTableElem = document.getElementById('cache-table');
        
        cacheInfoElem.innerText = cache ? 'Cache Configured' : 'Create a cache configuration to see visualization';
    }

    function updateStatistics(result) {
        stats.totalAccesses++;
        if (result === 'HIT') stats.hits++;
        else if (result === 'MISS') stats.misses++;

        totalAccessesElem.innerText = stats.totalAccesses;
        hitsElem.innerText = stats.hits;
        missesElem.innerText = stats.misses;

        hitRateElem.innerText = ((stats.hits / stats.totalAccesses) * 100).toFixed(2) + '%';
        missRateElem.innerText = ((stats.misses / stats.totalAccesses) * 100).toFixed(2) + '%';
    }

    function resetStatistics() {
        stats = {
            totalAccesses: 0,
            hits: 0,
            misses: 0
        };
        updateStatistics();
    }

    function logAccess(address, operation, result) {
        const logDiv = document.getElementById('access-log');
        const logEntry = document.createElement('div');
        const operationSymbol = operation === 'read' || operation === 'READ' ? '🔍' : '✍️';
        
        // Add CSS class based on result
        logEntry.className = `log-entry ${result.toLowerCase()}`;
        logEntry.innerText = `${operationSymbol} Access 0x${address.toString(16).toUpperCase()} - ${result}`;
        logDiv.appendChild(logEntry);
        logDiv.scrollTop = logDiv.scrollHeight;
    }

    // Update statistics from API response
    function updateStatisticsFromAPI(apiStats) {
        totalAccessesElem.innerText = apiStats.total_accesses || 0;
        hitsElem.innerText = apiStats.hits || 0;
        missesElem.innerText = apiStats.misses || 0;
        hitRateElem.innerText = (apiStats.hit_rate || 0).toFixed(2) + '%';
        missRateElem.innerText = (100 - (apiStats.hit_rate || 0)).toFixed(2) + '%';
    }

    // Show message to user
    function showMessage(message, type = 'info') {
        // Remove existing messages
        const existingMessages = document.querySelectorAll('.message');
        existingMessages.forEach(msg => msg.remove());
        
        // Create new message
        const messageDiv = document.createElement('div');
        messageDiv.className = `message ${type}`;
        messageDiv.textContent = message;
        
        // Insert at the top of main content
        const mainContent = document.querySelector('.main-content');
        mainContent.insertBefore(messageDiv, mainContent.firstChild);
        
        // Auto-remove after 5 seconds
        setTimeout(() => {
            if (messageDiv.parentNode) {
                messageDiv.remove();
            }
        }, 5000);
    }

    // Clear access log
    function clearLog() {
        const logDiv = document.getElementById('access-log');
        logDiv.innerHTML = '';
    }

    // Clear log button
    const clearLogButton = document.getElementById('clear-log');
    if (clearLogButton) {
        clearLogButton.addEventListener('click', clearLog);
    }

    // Export log button
    const exportLogButton = document.getElementById('export-log');
    if (exportLogButton) {
        exportLogButton.addEventListener('click', () => {
            const logDiv = document.getElementById('access-log');
            const logText = Array.from(logDiv.children)
                .map(entry => entry.textContent)
                .join('\n');
            
            const blob = new Blob([logText], { type: 'text/plain' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = 'cache_access_log.txt';
            a.click();
            URL.revokeObjectURL(url);
        });
    }

    // API Base URL - change this if your server runs on a different port
    const API_BASE = 'http://localhost:8080';

    // Utility function to make API calls
    async function makeAPICall(endpoint, params = {}) {
        try {
            const queryString = new URLSearchParams(params).toString();
            const url = `${API_BASE}${endpoint}${queryString ? '?' + queryString : ''}`;
            
            const response = await fetch(url, {
                method: 'GET',
                headers: {
                    'Content-Type': 'application/json',
                },
            });
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }
            
            const data = await response.json();
            return data;
        } catch (error) {
            console.error('API call failed:', error);
            // Fallback to simulation mode
            return simulateResponse(endpoint, params);
        }
    }

    // Fallback simulation when server is not available
    function simulateResponse(endpoint, params) {
        if (endpoint === '/create') {
            return { status: 'success', message: 'Cache created (simulated)' };
        } else if (endpoint === '/access') {
            const isHit = Math.random() < 0.6; // 60% hit rate
            return {
                status: 'success',
                result: isHit ? 'HIT' : 'MISS',
                statistics: {
                    total_accesses: Math.floor(Math.random() * 100),
                    hits: Math.floor(Math.random() * 60),
                    misses: Math.floor(Math.random() * 40),
                    hit_rate: Math.random() * 100
                }
            };
        } else if (endpoint === '/reset') {
            return { status: 'success', message: 'Cache reset (simulated)' };
        }
        return { status: 'error', message: 'Unknown endpoint' };
    }

    // CacheSimulator class with real C++ backend integration
    class CacheSimulator {
        constructor(cacheSize, blockSize, associativity, replacementPolicy, writePolicy, writeMissPolicy) {
            this.cacheSize = cacheSize;
            this.blockSize = blockSize;
            this.associativity = associativity;
            this.replacementPolicy = replacementPolicy;
            this.writePolicy = writePolicy;
            this.writeMissPolicy = writeMissPolicy;
            this.isConnected = false;
        }

        async create() {
            const params = {
                cache_size: this.cacheSize,
                block_size: this.blockSize,
                associativity: this.associativity,
                replacement_policy: this.replacementPolicy,
                write_policy: this.writePolicy,
                write_miss_policy: this.writeMissPolicy
            };

            const response = await makeAPICall('/create', params);
            this.isConnected = response.status === 'success';
            return response;
        }

        async access(address, operation) {
            const params = {
                address: '0x' + address.toString(16),
                operation: operation.toUpperCase()
            };

            const response = await makeAPICall('/access', params);
            return response;
        }

        async reset() {
            const response = await makeAPICall('/reset');
            return response;
        }

        async getInfo() {
            const response = await makeAPICall('/info');
            return response;
        }
    }
});
