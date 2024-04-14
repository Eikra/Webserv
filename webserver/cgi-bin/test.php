<?php
// Start the session
session_start();

// Set a session variable
$_SESSION['test'] = 'Hello, Session!';

// Check if the session variable is set and print its value
if(isset($_SESSION['test'])) {
    echo 'Session is working! Value of $_SESSION["test"] is: ' . $_SESSION['test'];
} else {
    echo 'Session is not working!';
}
?>
