<?php
// PHP CGI: stores names + emails into the SAME database as database.py.
//   GET  /cgi-bin/database.php   -> shows the form + all stored contacts
//   POST /cgi-bin/database.php   -> adds one contact, then shows the list
//
// Shares data/database/contacts.csv with database.py: identical header
// ("timestamp,name,email") and column order, standard CSV quoting, so both
// scripts read and write the same file interchangeably.

// php-cgi chdir()s into the script's own directory (<root>/data/cgi-bin),
// whereas the python CGI keeps the server's working directory (the project
// root). Rebuild an absolute path so BOTH scripts write the SAME database:
// getcwd() here is <root>/data/cgi-bin, so two levels up is the project root.
$PROJECT_ROOT = dirname(dirname(getcwd()));
$DB_DIR  = $PROJECT_ROOT . "/data/database";
$DB_PATH = $DB_DIR . "/contacts.csv";
$FIELDS  = array("timestamp", "name", "email");

// ---- read every stored contact as an array of assoc rows --------------------
function load_records($path, $fields) {
    $records = array();
    if (!file_exists($path)) {
        return $records;
    }
    $fh = fopen($path, "r");
    if ($fh === false) {
        return $records;
    }
    $header = fgetcsv($fh);          // skip the header row
    while (($row = fgetcsv($fh)) !== false) {
        if (count($row) < count($fields)) {
            continue;                // skip malformed / blank lines
        }
        // database.py writes CSV with \r\n line endings; strip any stray \r so
        // the two scripts stay interoperable on the shared file.
        $records[] = array(
            "timestamp" => rtrim($row[0], "\r"),
            "name"      => rtrim($row[1], "\r"),
            "email"     => rtrim($row[2], "\r"),
        );
    }
    fclose($fh);
    return $records;
}

// ---- append one contact, writing the header row if the file is new ----------
function save_record($dir, $path, $fields, $name, $email) {
    if (!is_dir($dir)) {
        mkdir($dir, 0755, true);
    }
    $is_new = !file_exists($path);
    $fh = fopen($path, "a");
    if ($fh === false) {
        return false;
    }
    if ($is_new) {
        fputcsv($fh, $fields);
    }
    fputcsv($fh, array(date("Y-m-d H:i:s"), $name, $email));
    fclose($fh);
    return true;
}

// ---- build the HTML page (all values escaped) -------------------------------
function render_page($records, $message = "") {
    $rows = "";
    foreach ($records as $r) {
        $rows .= "<tr><td>" . htmlspecialchars($r["timestamp"])
               . "</td><td>" . htmlspecialchars($r["name"])
               . "</td><td>" . htmlspecialchars($r["email"]) . "</td></tr>";
    }
    if ($rows === "") {
        $rows = "<tr><td colspan='3'><em>No contacts yet.</em></td></tr>";
    }
    return "<!DOCTYPE html>\n"
         . "<html>\n<head><meta charset=\"utf-8\"><title>Contact database</title></head>\n"
         . "<body>\n"
         . "  <h1>Contact database (PHP)</h1>\n"
         . "  " . $message . "\n"
         . "  <form method=\"post\">\n"
         . "    Name: <input name=\"username\" required />\n"
         . "    Email: <input type=\"email\" name=\"emailaddress\" required />\n"
         . "    <button type=\"submit\">Add</button>\n"
         . "  </form>\n"
         . "  <table border=\"1\" cellpadding=\"6\" style=\"margin-top:1em;border-collapse:collapse\">\n"
         . "    <tr><th>Added</th><th>Name</th><th>Email</th></tr>\n"
         . "    " . $rows . "\n"
         . "  </table>\n"
         . "</body>\n</html>";
}

// ---- main -------------------------------------------------------------------
$method = isset($_SERVER["REQUEST_METHOD"]) ? $_SERVER["REQUEST_METHOD"] : "GET";

if ($method === "POST") {
    // php-cgi fills $_POST from an application/x-www-form-urlencoded body
    $name  = isset($_POST["username"])     ? trim($_POST["username"])
           : (isset($_POST["name"])        ? trim($_POST["name"])  : "");
    $email = isset($_POST["emailaddress"]) ? trim($_POST["emailaddress"])
           : (isset($_POST["email"])       ? trim($_POST["email"]) : "");

    // basic validation
    if ($name === "" || $email === "" || strpos($email, "@") === false) {
        header("Status: 400 Bad Request");
        header("Content-Type: text/html; charset=utf-8");
        echo render_page(load_records($DB_PATH, $FIELDS),
            "<p style='color:red'>A name and a valid email are required.</p>");
        exit;
    }

    // avoid storing the same email twice
    $existing = load_records($DB_PATH, $FIELDS);
    foreach ($existing as $r) {
        if (strtolower($r["email"]) === strtolower($email)) {
            header("Content-Type: text/html; charset=utf-8");
            echo render_page($existing,
                "<p style='color:darkorange'>" . htmlspecialchars($email)
                . " is already in the database.</p>");
            exit;
        }
    }
    if (!save_record($DB_DIR, $DB_PATH, $FIELDS, $name, $email)) {
        header("Status: 500 Internal Server Error");
        header("Content-Type: text/html; charset=utf-8");
        echo render_page(
            $existing,
            "<p style='color:red'>ERROR: Could not write to the database.</p>"
        );
        exit;
    }

    header("Content-Type: text/html; charset=utf-8");
    echo render_page(
        load_records($DB_PATH, $FIELDS),
        "<p style='color:green'>Saved " . htmlspecialchars($name)
        . " &lt;" . htmlspecialchars($email) . "&gt;.</p>"
    );
}
// GET (or anything else): just display the current database
// header("Content-Type: text/html; charset=utf-8");
// echo render_page(load_records($DB_PATH, $FIELDS));
?>
