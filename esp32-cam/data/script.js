// Dashboard JavaScript for Posture Coach IoT

// Function to update the posture score gauge with animation
function updatePostureGauge(score) {
    const gauge = document.getElementById('posture-gauge');
    gauge.style.transition = 'width 0.5s';
    gauge.style.width = score + '%';
}

// Function to set color coding based on distance
function updateDistanceDisplay(distance) {
    const distanceDisplay = document.getElementById('distance-display');
    distanceDisplay.textContent = distance + ' cm';
    if (distance < 50) {
        distanceDisplay.style.color = 'green';
    } else if (distance < 100) {
        distanceDisplay.style.color = 'orange';
    } else {
        distanceDisplay.style.color = 'red';
    }
}

// Function to update session statistics
function updateSessionStats(stats) {
    document.getElementById('session-stats').textContent = `Total sessions: ${stats.total}, Avg score: ${stats.avgScore}`;
}

// Handle button clicks
document.getElementById('break-btn').onclick = function() { handleAction('break'); };
document.getElementById('snooze-btn').onclick = function() { handleAction('snooze'); };
document.getElementById('privacy-btn').onclick = function() { handleAction('privacy'); };

// Function to handle button actions
function handleAction(action) {
    // Logic to handle each action
    console.log(`Action: ${action}`);
    // Send a POST request based on action
    fetch(`/api/${action}`, { method: 'POST' });
}

// AJAX polling every 3 seconds
setInterval(() => {
    fetch('/api/posture/current')
        .then(response => response.json())
        .then(data => {
            updatePostureGauge(data.postureScore);
            updateDistanceDisplay(data.distance);
            updateSessionStats(data.sessionStats);
        })
        .catch(err => {
            console.error('Error fetching data:', err);
            // Handle error (e.g., display error message on UI)
        });
}, 3000);

// Settings modal open/close
const settingsModal = document.getElementById('settings-modal');
document.getElementById('settings-btn').onclick = function() {
    settingsModal.style.display = 'block';
};
document.getElementById('close-settings-btn').onclick = function() {
    settingsModal.style.display = 'none';
};

// Update settings via POST to /api/settings/update
document.getElementById('save-settings-btn').onclick = function() {
    const settings = { /* Gather settings from modal inputs */ };
    fetch('/api/settings/update', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(settings)
    }).then(response => {
        if (response.ok) {
            console.log('Settings updated!');
        } else {
            console.error('Failed to update settings');
        }
    });
};

// Smooth number transitions
function smoothNumberTransition(targetElement, endValue) {
    let startValue = 0;
    const duration = 2000;
    const stepTime = 50;
    const steps = Math.ceil(duration / stepTime);
    const increment = (endValue - startValue) / steps;
    let step = 0;
    const interval = setInterval(() => {
        startValue += increment;
        targetElement.textContent = Math.round(startValue);
        if (++step === steps) {
            clearInterval(interval);
        }
    }, stepTime);
}

// Local storage for user preferences
function saveUserPreference(key, value) {
    localStorage.setItem(key, JSON.stringify(value));
}

function loadUserPreference(key) {
    return JSON.parse(localStorage.getItem(key));
}
