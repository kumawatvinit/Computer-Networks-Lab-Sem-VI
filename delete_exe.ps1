# Find all files with the extensions ".exe" and with no extension in the current directory and all its subdirectories
$files = Get-ChildItem -Path . -File -Recurse | Where-Object { $_.Extension -eq ".exe" -or $_.Extension -eq "" }

# Loop through each file and delete it
foreach ($file in $files) {
    if ($file.Extension -eq ".exe" -or $file.Extension -eq "") {
        Remove-Item $file.FullName -Force
    }
}
