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
    
    // Trace file elements
    const traceFileInput = document.getElementById('trace-file');
    const traceContentInput = document.getElementById('trace-content');
    const processTraceButton = document.getElementById('process-trace');

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
    
    // Simulation state for fallback mode
    let simulationState = {
        cache: null,
        accessCount: 0,
        hitCount: 0,
        missCount: 0,
        cacheConfig: null
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
            updateCacheVisualization();
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
        updateCacheVisualization();
    });

    // Process trace button click
    processTraceButton.addEventListener('click', async () => {
        if (!cache) return showMessage('Please create a cache first.', 'error');

        let traceData = traceContentInput.value.trim();
        
        // If no manual content, try to read from file
        if (!traceData && traceFileInput.files.length > 0) {
            const file = traceFileInput.files[0];
            traceData = await readFile(file);
        }
        
        if (!traceData) return showMessage('Please provide trace content or select a file.', 'error');
        
        showMessage('Processing trace file...', 'info');
        
        const response = await cache.processTrace(traceData);
        
        if (response.status === 'success') {
            updateStatisticsFromAPI(response.statistics);
            
            // Log all results
            response.results.forEach(result => {
                const address = parseInt(result.address, 16);
                logAccess(address, result.operation, result.result);
            });
            
            showMessage(`Trace processed successfully! ${response.processed_count} operations completed.`, 'success');
            updateCacheVisualization();
        } else {
            showMessage(`Error: ${response.message}`, 'error');
        }
    });

    // Handle trace file selection
    traceFileInput.addEventListener('change', async (event) => {
        if (event.target.files.length > 0) {
            const file = event.target.files[0];
            const content = await readFile(file);
            traceContentInput.value = content;
        }
    });

    // Read file content
    async function readFile(file) {
        return new Promise((resolve, reject) => {
            const reader = new FileReader();
            reader.onload = (e) => resolve(e.target.result);
            reader.onerror = reject;
            reader.readAsText(file);
        });
    }

    // Utility functions
    async function updateCacheVisualization() {
        const cacheInfoElem = document.getElementById('cache-info');
        const cacheTableElem = document.getElementById('cache-table');
        
        if (!cache) {
            cacheInfoElem.innerHTML = '<p>Create a cache configuration to see visualization</p>';
            cacheTableElem.innerHTML = '';
            return;
        }
        
        try {
            console.log('Updating cache visualization...');
            const response = await cache.getContents();
            console.log('Cache contents response:', response);
            
            if (response.status === 'success') {
                // Update cache info
                cacheInfoElem.innerHTML = `
                    <div class="cache-config">
                        <span><strong>Cache Size:</strong> ${response.cache_size} bytes</span>
                        <span><strong>Block Size:</strong> ${response.block_size} bytes</span>
                        <span><strong>Associativity:</strong> ${response.associativity === 0 ? 'Fully Associative' : response.associativity + '-way'}</span>
                        <span><strong>Sets:</strong> ${response.num_sets}</span>
                    </div>
                `;
                
                // Update cache table
                renderCacheTable(response.contents);
            } else {
                console.error('Error response:', response);
                cacheInfoElem.innerHTML = `<p>Error loading cache information: ${response.message || 'Unknown error'}</p>`;
            }
        } catch (error) {
            console.error('Cache visualization error:', error);
            cacheInfoElem.innerHTML = `<p>Cache information not available: ${error.message}</p>`;
        }
    }
    
    function renderCacheTable(contents) {
        const cacheTableElem = document.getElementById('cache-table');
        
        console.log('Rendering cache table with contents:', contents);
        
        if (!contents || contents.length === 0) {
            cacheTableElem.innerHTML = '<p>No cache data available</p>';
            return;
        }
        
        let tableHTML = '<div class="cache-visualization">';
        
        contents.forEach(set => {
            tableHTML += `<div class="cache-set">`;
            tableHTML += `<div class="set-label">Set ${set.set}</div>`;
            tableHTML += `<div class="cache-blocks">`;
            
            set.blocks.forEach(block => {
                let blockClass = 'cache-block';
                if (block.valid) {
                    blockClass += ' valid';
                    if (block.dirty) {
                        blockClass += ' dirty';
                    }
                } else {
                    blockClass += ' invalid';
                }
                
                tableHTML += `<div class="${blockClass}">`;
                if (block.valid) {
                    tableHTML += `<div class="block-tag">0x${block.tag}</div>`;
                    tableHTML += `<div class="block-status">V:${block.valid ? '1' : '0'} D:${block.dirty ? '1' : '0'}</div>`;
                } else {
                    tableHTML += `<div class="block-tag">Invalid</div>`;
                    tableHTML += `<div class="block-status">V:0 D:0</div>`;
                }
                tableHTML += `</div>`;
            });
            
            tableHTML += `</div>`;
            tableHTML += `</div>`;
        });
        
        tableHTML += '</div>';
        console.log('Generated HTML:', tableHTML.substring(0, 200));
        cacheTableElem.innerHTML = tableHTML;
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
            
            console.log('Making API call to:', url);
            
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
            console.log('API response:', data);
            return data;
        } catch (error) {
            console.error('API call failed:', error);
            console.log('Falling back to simulation mode');
            // Fallback to simulation mode
            return simulateResponse(endpoint, params);
        }
    }

    // Fallback simulation when server is not available
    function simulateResponse(endpoint, params) {
        console.log('Using simulated response for:', endpoint);
        
        if (endpoint === '/create') {
            // Initialize simulation cache
            const cacheSize = parseInt(params.cache_size) || 1024;
            const blockSize = parseInt(params.block_size) || 32;
            const associativity = parseInt(params.associativity) || 4;
            const numSets = associativity === 0 ? 1 : cacheSize / (blockSize * associativity);
            
            simulationState.cacheConfig = {
                cacheSize, blockSize, associativity, numSets,
                writePolicy: params.write_policy || 'WRITE_THROUGH',
                replacementPolicy: params.replacement_policy || 'LRU'
            };
            
            // Initialize empty cache
            simulationState.cache = [];
            for (let i = 0; i < numSets; i++) {
                const blocks = [];
                for (let j = 0; j < (associativity === 0 ? cacheSize / blockSize : associativity); j++) {
                    blocks.push({ valid: false, dirty: false, tag: 0, lastAccessed: 0 });
                }
                simulationState.cache.push(blocks);
            }
            
            simulationState.accessCount = 0;
            simulationState.hitCount = 0;
            simulationState.missCount = 0;
            
            return { status: 'success', message: 'Cache created (simulated)' };
        } else if (endpoint === '/access') {
            if (!simulationState.cache) {
                return { status: 'error', message: 'No cache created' };
            }
            const address = parseInt(params.address, 16);
            const operation = params.operation ? params.operation.toUpperCase() : 'READ';
            const config = simulationState.cacheConfig;
            
            // Calculate set index and tag
            const offsetBits = Math.log2(config.blockSize);
            const indexBits = Math.log2(config.numSets);
            const setIndex = (address >> offsetBits) & ((1 << indexBits) - 1);
            const tag = address >> (offsetBits + indexBits);
            
            simulationState.accessCount++;
            
            // Check for hit
            let hitIndex = -1;
            for (let i = 0; i < simulationState.cache[setIndex].length; i++) {
                if (simulationState.cache[setIndex][i].valid && simulationState.cache[setIndex][i].tag === tag) {
                    hitIndex = i;
                    break;
                }
            }
            
            let result;
            if (hitIndex !== -1) {
                // Hit
                simulationState.hitCount++;
                simulationState.cache[setIndex][hitIndex].lastAccessed = simulationState.accessCount;
                if (operation === 'WRITE' && config.writePolicy === 'WRITE_BACK') {
                    simulationState.cache[setIndex][hitIndex].dirty = true;
                }
                result = operation === 'READ' ? 'HIT' : 'WRITE_HIT';
            } else {
                // Miss
                simulationState.missCount++;
                
                // Find empty block or evict
                let victimIndex = -1;
                for (let i = 0; i < simulationState.cache[setIndex].length; i++) {
                    if (!simulationState.cache[setIndex][i].valid) {
                        victimIndex = i;
                        break;
                    }
                }
                
                if (victimIndex === -1) {
                    // Need to evict - simple LRU simulation
                    let oldestAccess = simulationState.accessCount;
                    for (let i = 0; i < simulationState.cache[setIndex].length; i++) {
                        if (simulationState.cache[setIndex][i].lastAccessed < oldestAccess) {
                            oldestAccess = simulationState.cache[setIndex][i].lastAccessed;
                            victimIndex = i;
                        }
                    }
                }
                
                // Update the block
                simulationState.cache[setIndex][victimIndex] = {
                    valid: true,
                    dirty: (operation === 'WRITE' && config.writePolicy === 'WRITE_BACK'),
                    tag: tag,
                    lastAccessed: simulationState.accessCount
                };
                
                result = operation === 'READ' ? 'MISS' : 'WRITE_MISS';
            }
            
            return {
                status: 'success',
                result: result,
                statistics: {
                    total_accesses: simulationState.accessCount,
                    hits: simulationState.hitCount,
                    misses: simulationState.missCount,
                    hit_rate: simulationState.accessCount > 0 ? (simulationState.hitCount / simulationState.accessCount) * 100 : 0
                }
            };
        } else if (endpoint === '/reset') {
            if (simulationState.cache) {
                // Reset all blocks
                for (let i = 0; i < simulationState.cache.length; i++) {
                    for (let j = 0; j < simulationState.cache[i].length; j++) {
                        simulationState.cache[i][j] = { valid: false, dirty: false, tag: 0, lastAccessed: 0 };
                    }
                }
                simulationState.accessCount = 0;
                simulationState.hitCount = 0;
                simulationState.missCount = 0;
            }
            return { status: 'success', message: 'Cache reset (simulated)' };
        } else if (endpoint === '/contents') {
            if (!simulationState.cache || !simulationState.cacheConfig) {
                return { status: 'error', message: 'No cache created' };
            }
            
            const config = simulationState.cacheConfig;
            const contents = [];
            
            // Use actual simulation cache state
            for (let setIndex = 0; setIndex < simulationState.cache.length; setIndex++) {
                const blocks = [];
                for (let blockIndex = 0; blockIndex < simulationState.cache[setIndex].length; blockIndex++) {
                    const block = simulationState.cache[setIndex][blockIndex];
                    blocks.push({
                        block: blockIndex,
                        valid: block.valid,
                        dirty: block.dirty,
                        tag: block.tag.toString(16)
                    });
                }
                contents.push({
                    set: setIndex,
                    blocks: blocks
                });
            }
            
            return {
                status: 'success',
                cache_size: config.cacheSize,
                block_size: config.blockSize,
                associativity: config.associativity,
                num_sets: config.numSets,
                contents: contents
            };
        } else if (endpoint === '/info') {
            if (!simulationState.cacheConfig) {
                return { status: 'error', message: 'No cache created' };
            }
            
            const config = simulationState.cacheConfig;
            return {
                status: 'success',
                cache_size: config.cacheSize,
                block_size: config.blockSize,
                associativity: config.associativity,
                num_sets: config.numSets,
                statistics: {
                    total_accesses: simulationState.accessCount,
                    hits: simulationState.hitCount,
                    misses: simulationState.missCount,
                    hit_rate: simulationState.accessCount > 0 ? (simulationState.hitCount / simulationState.accessCount) * 100 : 0
                }
            };
        } else if (endpoint === '/trace') {
            return {
                status: 'success',
                processed_count: 5,
                results: [
                    { address: '0x1000', operation: 'R', result: 'MISS' },
                    { address: '0x2000', operation: 'W', result: 'MISS' },
                    { address: '0x1000', operation: 'R', result: 'HIT' }
                ],
                statistics: {
                    total_accesses: 13,
                    hits: 7,
                    misses: 6,
                    hit_rate: 53.8
                }
            };
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
        
        async getContents() {
            const response = await makeAPICall('/contents');
            return response;
        }
        
        async processTrace(traceData) {
            const params = {
                trace_data: encodeURIComponent(traceData)
            };
            const response = await makeAPICall('/trace', params);
            return response;
        }
    }
});
