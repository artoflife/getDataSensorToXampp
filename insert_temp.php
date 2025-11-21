<?php
header('Content-Type: application/json; charset=utf-8');

// Ambil parameter dari query string (GET)
$temp_raw = $_GET['temperature'] ?? null;
$hum_raw  = $_GET['humidity'] ?? null;

// Cek keberadaan parameter
if ($temp_raw === null || $hum_raw === null) {
    http_response_code(400);
    echo json_encode(['status'=>'error', 'message'=>'Parameter temperature dan humidity diperlukan.']);
    exit;
}

// Validasi numeric
if (!is_numeric($temp_raw) || !is_numeric($hum_raw)) {
    http_response_code(400);
    echo json_encode(['status'=>'error', 'message'=>'temperature dan humidity harus berupa angka.']);
    exit;
}

$temperature = floatval($temp_raw);
$humidity    = floatval($hum_raw);

// Konfigurasi DB
$servername   = "localhost";
$username     = "root";
$password     = "";
$database_name = "db_esp32";

// Koneksi MySQL (mysqli)
$connection = new mysqli($servername, $username, $password, $database_name);
if ($connection->connect_error) {
    http_response_code(500);
    echo json_encode(['status'=>'error', 'message'=>'Koneksi MySQL gagal: ' . $connection->connect_error]);
    exit;
}
$connection->set_charset('utf8mb4');

// Prepared statement untuk menyimpan data (waktu di-handle oleh DB)
$stmt = $connection->prepare("INSERT INTO tbl_temperature (nilai_temp, nilai_hum) VALUES (?, ?)");
if (!$stmt) {
    http_response_code(500);
    echo json_encode(['status'=>'error', 'message'=>'Prepare statement gagal: ' . $connection->error]);
    $connection->close();
    exit;
}
$stmt->bind_param('dd', $temperature, $humidity);

if ($stmt->execute()) {
    echo json_encode(['status'=>'ok', 'message'=>'Data tersimpan', 'temperature'=>$temperature, 'humidity'=>$humidity]);
} else {
    http_response_code(500);
    echo json_encode(['status'=>'error', 'message'=>'Query gagal: ' . $stmt->error]);
}

$stmt->close();
$connection->close();
?>
