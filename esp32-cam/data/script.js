// Global state
let privacyModeActive = false;
let lastUpdateTime = 0;
let connectionRetries = 0;
const MAX_RETRIES = 3;

// Initialize dashboard on page load
document.addEventListener('DOMContentLoaded', function() {
    console.log('Posture Coach Dashboard Initialized');
    startDataPolling();
    setupEventListeners();
    updateConnectionStatus(true);
});

// Start polling for posture data
function startDataPolling() {
    // Poll posture data every 3 seconds
    setInterval(fetchPostureData, 3000);
    
    // Poll session data every 5 seconds
    setInterval(fetchSessionData, 5000);
    
    // Initial fetch
    fetchPostureData();
    fetchSessionData();
}

// Setup event listeners
function setupEventListeners() {
    // Sensitivity slider
    const sensitivitySlider = document.getElementById('alertSensitivity');
    if (sensitivitySlider) {
        sensitivitySlider.addEventListener('input', function() {
            document.getElementById('sensitivityValue').textContent = this.value + '%';
        });
    }
    
    // Click outside modal to close
    window.addEventListener('click', function(event) {
        const modal = document.getElementById('settingsModal');
        if (event.target === modal) {
            closeSettings();
        }
    });
}

// Fetch current posture data from ESP32-CAM
async function fetchPostureData() {
    try {
        const response = await fetch('/api/posture/current');
        
        if (!response.ok) {
            throw new Error('HTTP error ' + response.status);
        }
        
        const data = await response.json();
        updatePostureDisplay(data);
        updateConnectionStatus(true);
        connectionRetries = 0;
        
    } catch (error) {
        console.error('Error fetching posture data:', error);
        handleConnectionError();
    }
}

// Fetch session data
async function fetchSessionData() {
    try {
        const response = await fetch('/api/session/active');
        
        if (!response.ok) {
            throw new Error('HTTP error ' + response.status);
        }
        
        const data = await response.json();
        updateSessionDisplay(data);
        
    } catch (error) {
        console.error('Error fetching session data:', error);
    }
}

// Update posture display with received data
function updatePostureDisplay(data) {
    // Update score
    const score = data.score || 0;
    document.getElementById('scoreNumber').textContent = score;
    
    // Update circular progress bar
    const circle = document.getElementById('progressCircle');
    const circumference = 565; // 2 * PI * 90
    const offset = circumference - (score / 100) * circumference;
    circle.style.strokeDashoffset = offset;
    
    // Color based on score
    if (score >= 90) {
        circle.style.stroke = '#44ff44';
        updateScoreStatus('Excellent', 'excellent');
    } else if (score >= 75) {
        circle.style.stroke = '#4444ff';
        updateScoreStatus('Good', 'good');
    } else if (score >= 60) {
        circle.style.stroke = '#ffaa00';
        updateScoreStatus('Fair', 'fair');
    } else {
        circle.style.stroke = '#ff4444';
        updateScoreStatus('Poor', 'poor');
    }
    
    // Update details
    const headTilt = data.headTilt || 0;
    document.getElementById('headTilt').textContent = headTilt.toFixed(1) + ' °';
    
    const shoulderAlign = data.shoulderAlign || 'unknown';
    document.getElementById('shoulderAlign').textContent = shoulderAlign;
    
    // Update distance if available
    const distance = data.distance || 0;
    if (distance > 0) {
        updateDistanceDisplay(distance);
    }
    
    lastUpdateTime = Date.now();
}

// Update score status
function updateScoreStatus(text, className) {
    const statusElement = document.getElementById('scoreStatus');
    statusElement.textContent = text;
    statusElement.className = 'score-status ' + className;
}

// Update distance display
function updateDistanceDisplay(distance) {
    document.getElementById('distanceNumber').textContent = distance.toFixed(1);
    
    // Update gauge fill (map 30-90cm to 0-100%)
    const minDist = 30;
    const maxDist = 90;
    const percentage = ((distance - minDist) / (maxDist - minDist)) * 100;
    const clampedPercentage = Math.max(0, Math.min(100, percentage));
    
    document.getElementById('distanceFill').style.width = clampedPercentage + '%';
    
    // Update status
    const statusElement = document.getElementById('distanceStatus');
    if (distance >= 50 && distance <= 70) {
        statusElement.textContent = 'Optimal Distance';
        statusElement.className = 'distance-status optimal';
    } else if (distance < 50) {
        statusElement.textContent = 'Too Close!';
        statusElement.className = 'distance-status close';
    } else {
        statusElement.textContent = 'Too Far';
        statusElement.className = 'distance-status far';
    }
}

// Update session display
function updateSessionDisplay(data) {
    // Duration
    const duration = data.duration || 0;
    document.getElementById('sessionDuration').textContent = formatDuration(duration);
    
    // Average score
    const avgScore = data.averageScore || 0;
    document.getElementById('avgScore').textContent = avgScore;
    
    // Alerts
    const alerts = data.alertsTriggered || 0;
    document.getElementById('alertCount').textContent = alerts;
    
    // Breaks
    const breaks = data.breaksTaken || 0;
    document.getElementById('breakCount').textContent = breaks;
}

// Format duration in seconds to HH:MM:SS
function formatDuration(seconds) {
    const hours = Math.floor(seconds / 3600);
    const minutes = Math.floor((seconds % 3600) / 60);
    const secs = seconds % 60;
    
    return [hours, minutes, secs]
        .map(v => v.toString().padStart(2, '0'))
        .join(':');
}

// Handle camera stream error
function handleStreamError() {
    const cameraStatus = document.getElementById('cameraStatus');
    const statusIndicator = cameraStatus.querySelector('.status-indicator');
    
    statusIndicator.classList.remove('active');
    cameraStatus.querySelector('span:last-child').textContent = 'Camera Offline';
    
    console.error('Camera stream failed to load');
}

// Handle connection error
function handleConnectionError() {
    connectionRetries++;
    
    if (connectionRetries >= MAX_RETRIES) {
        updateConnectionStatus(false);
        
        // Reset score display
        document.getElementById('scoreNumber').textContent = '--';
        document.getElementById('scoreStatus').textContent = 'Connection Lost';
        document.getElementById('distanceNumber').textContent = '--';
    }
}

// Update connection status indicator
function updateConnectionStatus(connected) {
    const statusElement = document.getElementById('connectionStatus');
    const statusText = statusElement.querySelector('.status-text');
    
    if (connected) {
        statusElement.classList.remove('disconnected');
        statusText.textContent = 'Connected';
    } else {
        statusElement.classList.add('disconnected');
        statusText.textContent = 'Disconnected';
    }
}

// Control button handlers
async function takeBreak() {
    try {
        console.log('Requesting break...');
        const response = await fetch('/api/break/start', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            }
        });
        
        if (response.ok) {
            const result = await response.json();
            console.log('Break started:', result);
            showNotification('Break started! Relax for 5 minutes', 'success');
        } else {
            throw new Error('Failed to start break');
        }
    } catch (error) {
        console.error('Error starting break:', error);
        showNotification('Failed to start break', 'error');
    }
}

async function snoozeAlerts() {
    try {
        console.log('Snoozing alerts for 5 minutes...');
        const response = await fetch('/api/alert/snooze', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            }
        });
        
        if (response.ok) {
            const result = await response.json();
            console.log('Alerts snoozed:', result);
            showNotification('Alerts snoozed for 5 minutes', 'info');
        } else {
            throw new Error('Failed to snooze alerts');
        }
    } catch (error) {
        console.error('Error snoozing alerts:', error);
        showNotification('Failed to snooze alerts', 'error');
    }
}

async function togglePrivacy() {
    try {
        console.log('Toggling privacy mode...');
        const response = await fetch('/api/privacy/toggle', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            }
        });
        
        if (!response.ok) {
            throw new Error('Failed to toggle privacy');
        }
        
        const result = await response.json();
        console.log('Privacy toggle result:', result);
        
        privacyModeActive = !privacyModeActive;
        
        const btn = document.getElementById('privacyBtn');
        const cameraStream = document.getElementById('cameraStream');
        
        if (privacyModeActive) {
            btn.classList.add('active');
            btn.querySelector('.btn-text').textContent = 'Privacy ON';
            cameraStream.style.display = 'none';
            showNotification('Privacy mode enabled', 'info');
        } else {
            btn.classList.remove('active');
            btn.querySelector('.btn-text').textContent = 'Privacy Mode';
            cameraStream.style.display = 'block';
            showNotification('Privacy mode disabled', 'info');
        }
    } catch (error) {
        console.error('Error toggling privacy:', error);
        showNotification('Failed to toggle privacy mode', 'error');
    }
}

// Settings modal functions
function openSettings() {
    const modal = document.getElementById('settingsModal');
    modal.classList.add('show');
    modal.style.display = 'flex';
}

function closeSettings() {
    const modal = document.getElementById('settingsModal');
    modal.classList.remove('show');
    modal.style.display = 'none';
}

async function saveSettings() {
    const settings = {
        alertSensitivity: parseInt(document.getElementById('alertSensitivity').value),
        breakInterval: parseInt(document.getElementById('breakInterval').value),
        minDistance: parseInt(document.getElementById('minDistance').value),
        maxDistance: parseInt(document.getElementById('maxDistance').value)
    };
    
    try {
        const response = await fetch('/api/settings/update', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify(settings)
        });
        
        if (response.ok) {
            console.log('Settings saved:', settings);
            showNotification('Settings saved successfully!', 'success');
            closeSettings();
        } else {
            throw new Error('Failed to save settings');
        }
    } catch (error) {
        console.error('Error saving settings:', error);
        showNotification('Failed to save settings', 'error');
    }
}

// Show notification (simple implementation)
function showNotification(message, type) {
    // Create notification element
    const notification = document.createElement('div');
    notification.textContent = message;
    notification.style.cssText = `
        position: fixed;
        top: 20px;
        left: 50%;
        transform: translateX(-50%);
        padding: 15px 30px;
        border-radius: 10px;
        color: white;
        font-weight: bold;
        z-index: 9999;
        animation: slideDown 0.3s ease;
    `;
    
    // Set color based on type
    if (type === 'success') {
        notification.style.background = 'linear-gradient(135deg, #11998e 0%, #38ef7d 100%)';
    } else if (type === 'error') {
        notification.style.background = 'linear-gradient(135deg, #ff4444 0%, #ff6666 100%)';
    } else {
        notification.style.background = 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)';
    }
    
    document.body.appendChild(notification);
    
    // Remove after 3 seconds
    setTimeout(() => {
        notification.style.animation = 'slideUp 0.3s ease';
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

// Add animation styles
const style = document.createElement('style');
style.textContent = `
    @keyframes slideDown {
        from {
            transform: translate(-50%, -100%);
            opacity: 0;
        }
        to {
            transform: translate(-50%, 0);
            opacity: 1;
        }
    }
    
    @keyframes slideUp {
        from {
            transform: translate(-50%, 0);
            opacity: 1;
        }
        to {
            transform: translate(-50%, -100%);
            opacity: 0;
        }
    }
`;
document.head.appendChild(style);

console.log('Dashboard script loaded successfully');
