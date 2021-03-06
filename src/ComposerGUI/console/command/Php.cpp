#include <QSettings>
#include <QFileInfo>
#include <QProcess>
#include <QIODevice>

#include "Php.h"
#include "globals.h"

namespace composer_gui
{
namespace console
{
namespace command
{

Php::Php(QObject *parent) : AbstractCommand(parent)
{

}

void Php::run()
{
    emit started(getCommand());

    QSettings settings(COMPOSERGUI_ORGANIZATION_NAME, COMPOSERGUI_SETTINGS_APPLICATION);

    QString phpPath = settings.value("php_binary_path").toString();
    if (!QFileInfo(phpPath).exists()) {
        emit message("PHP binary not found at " + phpPath);
        emit finished(getCommand());
        return;
    }

    QProcess proc;
    proc.setWorkingDirectory(getCommand().getExecutionDirectory());
    proc.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&proc, &QProcess::readyReadStandardOutput, [this, &proc]
    {
       QString line = QString::fromStdString(proc.readAll().toStdString()).trimmed();
       if (!line.isEmpty()) {
           emit message(line);
       }
    });

    proc.start(phpPath, getCommand().getArguments(), QIODevice::ReadWrite | QIODevice::Text);
    proc.waitForFinished(-1);

    emit finished(getCommand());
}

} // command
} // console
} // composer_gui
