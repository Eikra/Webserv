#!/usr/bin/php-cgi
<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $deleteDir = __DIR__ . '/uploads/';  // Use the absolute path to the uploads directory
    $filename = isset($_POST['filename']) ? basename($_POST['filename']) : null;

    if ($filename !== null) {
        $filePath = $deleteDir . $filename;

        if (file_exists($filePath)) {
            if (unlink($filePath)) {
                echo "<p>File deleted successfully: " . htmlspecialchars($filename) . "</p>";
            } else {
                echo "<p>Error deleting file.</p>";
            }
        } else {
            echo "<p>File not found: " . htmlspecialchars($filename) . "</p>";
        }
    } else {
        echo "<p>Invalid filename.</p>";
    }
}
?>
