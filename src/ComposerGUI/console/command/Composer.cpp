#include <QSettings>
#include <QProcess>
#include <QFileInfo>
#include <QIODevice>
#include <QtGlobal>

#include "globals.h"
#include "Composer.h"

namespace composer_gui
{
namespace console
{
namespace command
{

Composer::Composer(QObject *parent)
    : AbstractCommand(parent),
      m_settings(COMPOSERGUI_ORGANIZATION_NAME, COMPOSERGUI_SETTINGS_APPLICATION)
{

}

void Composer::run()
{
    emit started(getCommand());

    QString phpPath = m_settings.value("php_binary_path").toString();
    QString composerPath = m_settings.value("composer_binary_path").toString();

    if (!QFileInfo(phpPath).exists()) {
        emit message("PHP binary not found at " + phpPath);
        emit finished(getCommand());
        return;
    }

    if (!QFileInfo(composerPath).exists()) {
        emit message("Composer binary not found at " + composerPath);
        emit finished(getCommand());
        return;
    }

    qputenv("http_proxy", m_settings.value("var_http_proxy").toByteArray());
    qputenv("https_proxy", m_settings.value("var_https_proxy").toByteArray());

    QProcess proc;

    proc.setWorkingDirectory(getCommand().getExecutionDirectory());
    proc.setProcessChannelMode(QProcess::MergedChannels);
    QObject::connect(&proc, &QProcess::readyReadStandardOutput, [this, &proc]
    {
       QString line = QString::fromStdString(proc.readAll().toStdString()).trimmed();
       if (!line.isEmpty()) {
           emit message(line.replace('\b', ""));
       }
    });

    proc.start(phpPath,
               QStringList() << composerPath << getCommand().getArguments(),
               QIODevice::ReadWrite | QIODevice::Text);
    proc.waitForFinished(-1);

    emit finished(getCommand());
}

} // command
} // console
} // composer_gui
