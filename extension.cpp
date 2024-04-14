#include "config.hpp"

std::string Client::getContentTypeByExtension(const std::string& extension) 
{
    std::map<std::string, std::string> extensionToContentType;
    extensionToContentType[".html"] = "text/html";
        extensionToContentType[".css"] = "text/css";
        extensionToContentType[".js"] = "application/javascript";
        extensionToContentType[".jpeg"] = "image/jpeg";
        extensionToContentType[".png"] = "image/png";
        extensionToContentType[".gif"] = "image/gif";
        extensionToContentType[".mp4"] = "video/mp4";
        extensionToContentType[".pdf"] = "application/pdf";
        extensionToContentType[".txt"] = "text/plain";
        extensionToContentType[".xml"] = "application/xml";
        extensionToContentType[".json"] = "application/json";
        extensionToContentType[".csv"] = "text/csv";
        extensionToContentType[".doc"] = "application/msword";
        extensionToContentType[".docx"] = "application/vnd.openxmlformats-officedocument.wordprocessingml.document";
        extensionToContentType[".xls"] = "application/vnd.ms-excel";
        extensionToContentType[".xlsx"] = "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet";
        extensionToContentType[".ppt"] = "application/vnd.ms-powerpoint";
        extensionToContentType[".pptx"] = "application/vnd.openxmlformats-officedocument.presentationml.presentation";
        extensionToContentType[".zip"] = "application/zip";
        extensionToContentType[".tar"] = "application/x-tar";
        extensionToContentType[".gz"] = "application/gzip";
        extensionToContentType[".mp3"] = "audio/mpeg";
        extensionToContentType[".ogg"] = "audio/ogg";
        extensionToContentType[".woff"] = "font/woff";
        extensionToContentType[".woff2"] = "font/woff2";
        extensionToContentType[".ico"] = "image/x-icon";
        extensionToContentType[".svg"] = "image/svg+xml";
        extensionToContentType[".avi"] = "video/x-msvideo";
        extensionToContentType[".mov"] = "video/quicktime";
        extensionToContentType[".bmp"] = "image/bmp";
        extensionToContentType[".webp"] = "image/webp";
        extensionToContentType[".tiff"] = "image/tiff";
        extensionToContentType[".ejs"] = "application/vnd.ms-excel.sheet.macroEnabled.12";
        extensionToContentType[".mpg"] = "video/mpeg";
        extensionToContentType[".webm"] = "video/webm";
        extensionToContentType[".ics"] = "text/calendar";
        extensionToContentType[".java"] = "text/x-java-source";
        extensionToContentType[".cpp"] = "text/x-c++src";
        extensionToContentType[".php"] = "text/php";
        extensionToContentType[".yml"] = "application/x-yaml";
        extensionToContentType[".sql"] = "application/sql";
        extensionToContentType[".log"] = "text/plain";
        extensionToContentType[".bat"] = "application/bat";
        extensionToContentType[".exe"] = "application/octet-stream";
        extensionToContentType[".dll"] = "application/x-msdownload";
        extensionToContentType[".class"] = "application/java-vm";
        extensionToContentType[".psd"] = "image/vnd.adobe.photoshop";
        extensionToContentType[".wav"] = "audio/wav";
        extensionToContentType[".flac"] = "audio/flac";
        extensionToContentType[".jpg"] = "image/jpeg";

    std::string lowercaseExtension = toLowerCase(extension);

    std::map<std::string, std::string>::const_iterator it = extensionToContentType.find(lowercaseExtension);
    if (it != extensionToContentType.end()) {
        return it->second;
    } else {
        return "text/plain";
    }
}
std::string Client::getExtensionByContentType(const std::string& contentType) 
{
    std::map<std::string, std::string>  contentTypeToExtension;

    contentTypeToExtension["text/html"] = ".html";
    contentTypeToExtension["text/css"] = ".css";
    contentTypeToExtension["application/javascript"] = ".js";
    contentTypeToExtension["image/jpeg"] = ".jpeg";
    contentTypeToExtension["image/png"] = ".png";
    contentTypeToExtension["image/gif"] = ".gif";
    contentTypeToExtension["video/mp4"] = ".mp4";
    contentTypeToExtension["application/pdf"] = ".pdf";
    contentTypeToExtension["text/plain"] = ".txt";
    contentTypeToExtension["application/xml"] = ".xml";
    contentTypeToExtension["application/json"] = ".json";
    contentTypeToExtension["text/csv"] = ".csv";
    contentTypeToExtension["application/msword"] = ".doc";
    contentTypeToExtension["application/vnd.openxmlformats-officedocument.wordprocessingml.document"] = ".docx";
    contentTypeToExtension["application/vnd.ms-excel"] = ".xls";
    contentTypeToExtension["application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"] = ".xlsx";
    contentTypeToExtension["application/vnd.ms-powerpoint"] = ".ppt";
    contentTypeToExtension["application/vnd.openxmlformats-officedocument.presentationml.presentation"] = ".pptx";
    contentTypeToExtension["application/zip"] = ".zip";
    contentTypeToExtension["application/x-tar"] = ".tar";
    contentTypeToExtension["application/gzip"] = ".gz";
    contentTypeToExtension["audio/mpeg"] = ".mp3";
    contentTypeToExtension["audio/ogg"] = ".ogg";
    contentTypeToExtension["font/woff"] = ".woff";
    contentTypeToExtension["font/woff2"] = ".woff2";
    contentTypeToExtension["text/x-c"] = ".cpp";
    contentTypeToExtension["text/x-java-source"] = ".java";
    contentTypeToExtension["application/x-yaml"] = ".yml";
    contentTypeToExtension["application/sql"] = ".sql";
    contentTypeToExtension["text/calendar"] = ".ics";
    contentTypeToExtension["text/php"] = ".php";
    contentTypeToExtension["image/x-icon"] = ".ico";
    contentTypeToExtension["image/svg+xml"] = ".svg";
    contentTypeToExtension["video/x-msvideo"] = ".avi";
    contentTypeToExtension["video/quicktime"] = ".mov";
    contentTypeToExtension["image/bmp"] = ".bmp";
    contentTypeToExtension["image/webp"] = ".webp";
    contentTypeToExtension["image/tiff"] = ".tiff";
    contentTypeToExtension["application/vnd.ms-excel.sheet.macroEnabled.12"] = ".ejs";
    contentTypeToExtension["video/mpeg"] = ".mpg";
    contentTypeToExtension["video/webm"] = ".webm";
    contentTypeToExtension["text/x-java-source"] = ".java";
    contentTypeToExtension["text/x-c++src"] = ".cpp";
    contentTypeToExtension["text/php"] = ".php";
    contentTypeToExtension["application/x-yaml"] = ".yml";
    contentTypeToExtension["application/sql"] = ".sql";
    contentTypeToExtension["text/calendar"] = ".ics";
    contentTypeToExtension["application/bat"] = ".bat";
    contentTypeToExtension["application/octet-stream"] = ".py";
    contentTypeToExtension["application/x-msdownload"] = ".dll";
    contentTypeToExtension["application/java-vm"] = ".class";
    contentTypeToExtension["image/vnd.adobe.photoshop"] = ".psd";
    contentTypeToExtension["audio/wav"] = ".wav";
    contentTypeToExtension["audio/flac"] = ".flac";
    contentTypeToExtension["application/x-httpd-php"] = ".php";
    contentTypeToExtension["image/jpeg"] = ".jpg";
        
    std::string lowercaseContentType = toLowerCase(contentType);

    std::map<std::string, std::string>::const_iterator it = contentTypeToExtension.find(lowercaseContentType);
    if (it != contentTypeToExtension.end()) {
        return it->second;
    } else {
        return "";
    }
}
std::string Client::toLowerCase(const std::string& str) 
{
    std::string result = str;
    for (size_t i = 0; i < result.size(); ++i) {
        result[i] = tolower(result[i]);
    }
    return result;
}
std::string Client::concatenatePaths(const std::string& path1, const std::string& path2) 
{
    std::string tmp = "." + resource;
    if (tmp == filePath && tmp.size() == filePath.size() )
    {
        return path2;
    }
    else
    {
         if (path1.empty() || path1[path1.size() - 1] == '/')
        return path1 + path2;
    else
        return path1 + '/' + path2;
    }
       
}