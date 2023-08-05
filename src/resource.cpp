#include "resource.h"

std::string getResourcePath(const std::string& resourceName) {
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFStringRef resourceStringRef = CFStringCreateWithCString(nullptr, resourceName.c_str(), kCFStringEncodingUTF8);
    CFURLRef resourceURL = CFBundleCopyResourceURL(mainBundle, resourceStringRef, nullptr, nullptr);
    CFRelease(resourceStringRef);

    if (!resourceURL) {
        // Resource not found
        return "";
    }

    char path[PATH_MAX];
    if (CFURLGetFileSystemRepresentation(resourceURL, true, reinterpret_cast<UInt8*>(path), PATH_MAX)) {
        CFRelease(resourceURL);
        return path;
    }

    CFRelease(resourceURL);
    return "";
}
