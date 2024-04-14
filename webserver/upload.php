#!/usr/bin/php-cgi
<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    $uploadDir = __DIR__ . '/uploads/';  // Use the absolute path to the uploads directory
    $uploadedFile = $uploadDir . basename($_FILES['file']['name']);
    //echo $uploadedFile;
    $maxFileSize = 1000000;  // Set the maximum file size in bytes

    // Check if the file size exceeds the limit
    if ($_FILES['file']['size'] > $maxFileSize) {
        echo "<p>Error: File size exceeds the limit ({$maxFileSize} bytes).</p>";
    } else {
        // Proceed with the file upload
        if (move_uploaded_file($_FILES['file']['tmp_name'], $uploadedFile)) {
            echo "<p>File uploaded successfully: " . htmlspecialchars(basename($_FILES['file']['name'])) . "</p>";
        } else {
            echo "<p>Error uploading file.</p>";
        }
    }
}
?>
