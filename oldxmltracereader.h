#ifndef OLDXMLTRACEREADER_H_
#define OLDXMLTRACEREADER_H_

#include "tracereader.h"
#include "sourcefileidaccumulator.h"
#include <QXmlStreamReader>

namespace Templar {

struct OldXMLTraceReader : TraceReader {
    OldXMLTraceReader(TraceEntry &target) : TraceReader{target} {}

    static SourceFileIdAccumulator::SourceFiles
    readSourceFiles(const QString &fileName);

    BuildReturn build(QString fileName) override;
  private:
    static SourceFileLocation locationFromXML(QXmlStreamReader &xml);
};

}

#endif /* OLDXMLTRACEREADER_H_ */
