#pragma once

class QString;

class XMLParser {
  public:
    XMLParser( const QString& filename, bool generate);
    bool parse();
};
