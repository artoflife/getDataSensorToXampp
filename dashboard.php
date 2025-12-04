<?php
// Konfigurasi koneksi database
$servername = "localhost";
$username   = "root";
$password   = "";
$database   = "db_esp32";

// Koneksi ke database
$connection = new mysqli($servername, $username, $password, $database);
if ($connection->connect_error) {
    die("Koneksi gagal: " . $connection->connect_error);
}
$connection->set_charset('utf8mb4');

// Query ambil data terbaru (misalnya 20 data terakhir)
$sql = "SELECT id_temp, nilai_temp, nilai_hum, waktu FROM tbl_temperature ORDER BY id_temp DESC LIMIT 20";
$result = $connection->query($sql);
?>

<!DOCTYPE html>
<html lang="id">
<head>
    <meta charset="UTF-8">
    <title>Dashboard Monitoring Suhu & Kelembaban</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 40px;
            background-color: #f4f6f8;
            color: #333;
        }
        h1 {
            text-align: center;
            color: #1a73e8;
        }
        table {
            width: 80%;
            margin: 20px auto;
            border-collapse: collapse;
            box-shadow: 0 0 10px rgba(0,0,0,0.1);
            background: white;
        }
        th, td {
            padding: 10px 15px;
            border-bottom: 1px solid #ddd;
            text-align: center;
        }
        th {
            background-color: #1a73e8;
            color: white;
        }
        tr:hover {
            background-color: #f1f1f1;
        }
        .refresh {
            display: block;
            width: 200px;
            margin: 20px auto;
            text-align: center;
            padding: 10px;
            background-color: #1a73e8;
            color: white;
            text-decoration: none;
            border-radius: 5px;
        }
        .refresh:hover {
            background-color: #155fc0;
        }
    </style>
</head>
<body>

<h1>🌡️ Dashboard Monitoring Suhu & Kelembaban</h1>

<table>
    <tr>
        <th>ID</th>
        <th>Suhu (°C)</th>
        <th>Kelembaban (%)</th>
        <th>Waktu</th>
    </tr>

    <?php
    if ($result->num_rows > 0) {
        while ($row = $result->fetch_assoc()) {
            echo "<tr>";
            echo "<td>".$row['id_temp']."</td>";
            echo "<td>".$row['nilai_temp']."</td>";
            echo "<td>".$row['nilai_hum']."</td>";
            echo "<td>".$row['waktu']."</td>";
            echo "</tr>";
        }
    } else {
        echo "<tr><td colspan='4'>Belum ada data</td></tr>";
    }
    ?>
</table>

<a href="dashboard.php" class="refresh">🔄 Refresh Data</a>

</body>
</html>

<?php
$connection->close();
?>
