<?php
session_start();

// Check if the session count variable exists
if (!isset($_SESSION['page_visit_count'])) {
    echo "first time ";
    $_SESSION['page_visit_count'] = 1; // Initialize the count if it doesn't exist
} else {
    
    $_SESSION['page_visit_count']++; // Increment the count if it exists
}

// Display the page visit count
echo "You have visited this page " . $_SESSION['page_visit_count'] . " times.";
?>
