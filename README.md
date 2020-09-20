# WebView2_Sample
WebView2 sample

# Build
git clone https://github.com/microsoft/vcpkg

.\vcpkg\bootstrap-vcpkg.bat
# Build boost & cpprestsdk
vcpkg.exe install boost:x64-windows cpprestsdk:x64-windows
# Create the nuget package
vcpkg export boost:x64-windows cpprestsdk:x64-windows --nuget --output=boost_cpprestsdk_x64_v2019.1
# import
With a project open, go to Tools->NuGet Package Manager->Package Manager Console and paste:
Install-Package boost_cpprestsdk_x64_v2019.1 -Source "<path>\vcpkg"
