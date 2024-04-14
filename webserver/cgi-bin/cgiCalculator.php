#!/usr/bin/php-cgi
<?php
$queryString = getenv("QUERY_STRING");
parse_str($queryString, $params);

// Check if form is submitted
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // Retrieve user input from the form
    $num1 = isset($_POST['num1']) ? floatval($_POST['num1']) : null;
    $num2 = isset($_POST['num2']) ? floatval($_POST['num2']) : null;
    $operation = isset($_POST['operation']) ? $_POST['operation'] : null;

    // Validate input
    if ($num1 !== null && $num2 !== null && $operation !== null) {
        // Perform the selected operation
        switch ($operation) {
            case 'add':
                $result = $num1 + $num2;
                $operationSymbol = '+';
                break;
            case 'subtract':
                $result = $num1 - $num2;
                $operationSymbol = '-';
                break;
            case 'multiply':
                $result = $num1 * $num2;
                $operationSymbol = '*';
                break;
            case 'divide':
                if ($num2 != 0) {
                    $result = $num1 / $num2;
                    $operationSymbol = '/';
                } else {
                    $error = 'Error: Division by zero.';
                }
                break;
            default:
                $error = 'Error: Invalid operation.';
        }
    } else {
        $error = 'Error: Missing or invalid input parameters.';
    }
}
?>

<html>
<head>
    <title>CGI Calculator</title>
</head>
<body>
    <h1>CGI Calculator</h1>

    <?php
    // Display the result or an error message
    if (isset($result)) {
        echo "<p>Result: {$num1} {$operationSymbol} {$num2} = {$result}</p>";
    } elseif (isset($error)) {
        echo "<p>{$error}</p>";
    }
    ?>

    <form method="post" action="">
        <label for="num1">Number 1:</label>
        <input type="text" name="num1" required>

        <label for="num2">Number 2:</label>
        <input type="text" name="num2" required>

        <label for="operation">Operation:</label>
        <select name="operation" required>
            <option value="add">Addition</option>
            <option value="subtract">Subtraction</option>
            <option value="multiply">Multiplication</option>
            <option value="divide">Division</option>
        </select>

        <button type="submit">Calculate</button>
    </form>
</body>
</html>
