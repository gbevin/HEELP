#include "HeelpLogger.h"

using namespace heelp;

HeelpLogger::HeelpLogger(int identifier) : FileLogger(File::getCurrentWorkingDirectory().getChildFile("heelp"+String(identifier)+".log"),
                                        String(ProjectInfo::projectName)+" "+ProjectInfo::versionString, 128 * 1024)
{
}

void HeelpLogger::logMessage(const String& message)
{
    FileLogger::logMessage(message);
}
