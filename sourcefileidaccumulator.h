#ifndef SOURCEFILEIDACCUMULATOR_H_
#define SOURCEFILEIDACCUMULATOR_H_

#include <QFileInfo>
#include <QDir>

namespace Templar {

struct SourceFileLocation {
    QString filePath;
    unsigned int line, column;
};

struct SourceFileIdAccumulator {

    SourceFileIdAccumulator(QString dirPath) : dirPath(dirPath) {}

    SourceLocation addLocation(SourceFileLocation const &location) {
        QFileInfo fileInfo(location.filePath);
        if(fileInfo.isRelative()) {
          fileInfo = QDir(dirPath).filePath(location.filePath);
        }
        QString sourceFile = fileInfo.canonicalFilePath();
        auto iterator = sourceFileIds.find(sourceFile);
        if (iterator == sourceFileIds.end()) {
            iterator =
                sourceFileIds.insert(std::make_pair(sourceFile,
                                                    sourceFiles.size())).first;
            sourceFiles.push_back(sourceFile);
        }
        return SourceLocation{iterator->second, location.line, location.column};
    }

    using SourceFiles = std::vector<QString>;
    SourceFiles sourceFiles;
    std::map<QString, size_t> sourceFileIds;
    QString dirPath;
};

}

#endif /* SOURCEFILEIDACCUMULATOR_H_ */
