$dllPath = "c:\Users\SL4X3\repos\ffxi-helper-service\libs\elite\EliteMMO.API.dll"
$outputPath = "c:\Users\SL4X3\repos\ffxi-helper-service\libs\elite\DECOMPILED_API.md"

$assembly = [System.Reflection.Assembly]::LoadFrom($dllPath)

$sb = [System.Text.StringBuilder]::new()
[void]$sb.AppendLine("# EliteMMO.API.dll - Complete Decompiled API Documentation")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("**Generated:** $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')")
[void]$sb.AppendLine("**Method:** .NET Reflection Decompilation")
[void]$sb.AppendLine("**Permission:** Granted by original creator")
[void]$sb.AppendLine("")
[void]$sb.AppendLine("---")
[void]$sb.AppendLine("")

$types = $assembly.GetTypes() | Where-Object { $_.IsPublic } | Sort-Object Name

foreach ($type in $types) {
    [void]$sb.AppendLine("## $($type.Name)")
    [void]$sb.AppendLine("")
    [void]$sb.AppendLine("**Namespace:** EliteMMO.API")
    [void]$sb.AppendLine("**Full Name:** $($type.FullName)")

    if ($type.BaseType) {
        [void]$sb.AppendLine("**Base Type:** $($type.BaseType.Name)")
    }

    [void]$sb.AppendLine("")

    if ($type.IsEnum) {
        [void]$sb.AppendLine("### Enum Values")
        [void]$sb.AppendLine("")
        $enumValues = [Enum]::GetNames($type)
        foreach ($enumName in $enumValues) {
            try {
                $enumValue = [Enum]::Parse($type, $enumName)
                $intVal = [Convert]::ToInt64($enumValue)
                [void]$sb.AppendLine("- ``$enumName`` = $intVal")
            } catch {
                [void]$sb.AppendLine("- ``$enumName``")
            }
        }
        [void]$sb.AppendLine("")
    }

    # Constructors
    $constructors = $type.GetConstructors() | Where-Object { $_.IsPublic }
    if ($constructors.Count -gt 0) {
        [void]$sb.AppendLine("### Constructors")
        [void]$sb.AppendLine("")
        foreach ($ctor in $constructors) {
            $params = ($ctor.GetParameters() | ForEach-Object { "$($_.ParameterType.Name) $($_.Name)" }) -join ', '
            [void]$sb.AppendLine("``````csharp")
            [void]$sb.AppendLine("$($type.Name)($params)")
            [void]$sb.AppendLine("``````")
            [void]$sb.AppendLine("")
        }
    }

    # Properties
    $properties = $type.GetProperties() | Where-Object { $_.DeclaringType.Name -eq $type.Name } | Sort-Object Name
    if ($properties.Count -gt 0) {
        [void]$sb.AppendLine("### Properties")
        [void]$sb.AppendLine("")
        [void]$sb.AppendLine("| Property | Type | Get | Set |")
        [void]$sb.AppendLine("|----------|------|-----|-----|")
        foreach ($prop in $properties) {
            $canRead = if ($prop.CanRead) { '✓' } else { '' }
            $canWrite = if ($prop.CanWrite) { '✓' } else { '' }
            [void]$sb.AppendLine("| ``$($prop.Name)`` | ``$($prop.PropertyType.Name)`` | $canRead | $canWrite |")
        }
        [void]$sb.AppendLine("")
    }

    # Methods (excluding property getters/setters)
    $methods = $type.GetMethods() | Where-Object {
        $_.IsPublic -and
        -not $_.IsSpecialName -and
        $_.DeclaringType.Name -eq $type.Name
    } | Sort-Object Name

    if ($methods.Count -gt 0) {
        [void]$sb.AppendLine("### Methods")
        [void]$sb.AppendLine("")
        foreach ($method in $methods) {
            $params = ($method.GetParameters() | ForEach-Object { "$($_.ParameterType.Name) $($_.Name)" }) -join ', '
            $returnType = $method.ReturnType.Name
            [void]$sb.AppendLine("#### $($method.Name)")
            [void]$sb.AppendLine("")
            [void]$sb.AppendLine("``````csharp")
            [void]$sb.AppendLine("$returnType $($method.Name)($params)")
            [void]$sb.AppendLine("``````")
            [void]$sb.AppendLine("")
        }
    }

    [void]$sb.AppendLine("---")
    [void]$sb.AppendLine("")
}

$sb.ToString() | Out-File -FilePath $outputPath -Encoding UTF8
Write-Host "Complete API documentation exported to: $outputPath"
Write-Host "Total types documented: $($types.Count)"
