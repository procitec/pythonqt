/****************************************************************************
**
** Copyright (C) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Script Generator project on Qt Labs.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "typesystem.h"
#include "generator.h"
#include "xmlparser.h"

#include "customtypes.h"

#include <reporthandler.h>

#include <memory>

#include <QtXml>

QString strings_Object = QLatin1String("Object");
QString strings_String = QLatin1String("String");
QString strings_Thread = QLatin1String("Thread");
QString strings_char = QLatin1String("char");
QString strings_java_lang = QLatin1String("java.lang");
QString strings_jchar = QLatin1String("jchar");
QString strings_jobject = QLatin1String("jobject");

static void addRemoveFunctionToTemplates(TypeDatabase *db);

class StackElement
{
    public:
    enum ElementType {
        None = 0x0,

        // Type tags (0x1, ... , 0xff)
        ObjectTypeEntry      = 0x1,
        ValueTypeEntry       = 0x2,
        InterfaceTypeEntry   = 0x3,
        NamespaceTypeEntry   = 0x4,
        ComplexTypeEntryMask = 0xf,

        // Non-complex type tags (0x10, 0x20, ... , 0xf0)
        PrimitiveTypeEntry   = 0x10,
        EnumTypeEntry        = 0x20,
        TypeEntryMask        = 0xff,

        // Simple tags (0x100, 0x200, ... , 0xf00)
        ExtraIncludes               = 0x100,
        Include                     = 0x200,
        ModifyFunction              = 0x300,
        ModifyField                 = 0x400,
        Root                        = 0x500,
        CustomMetaConstructor       = 0x600,
        CustomMetaDestructor        = 0x700,
        ArgumentMap                 = 0x800,
        SuppressedWarning           = 0x900,
        Rejection                   = 0xa00,
        LoadTypesystem              = 0xb00,
        RejectEnumValue             = 0xc00,
        Template                    = 0xd00,
        TemplateInstanceEnum        = 0xe00,
        Replace                     = 0xf00,
        SimpleMask                  = 0xf00,

        // Code snip tags (0x1000, 0x2000, ... , 0xf000)
        InjectCode =           0x1000,
        InjectCodeInFunction = 0x2000,
        CodeSnipMask =         0xf000,

        // Function modifier tags (0x010000, 0x020000, ... , 0xf00000)
        Access                   = 0x010000,
        Removal                  = 0x020000,
        Rename                   = 0x040000,
        ModifyArgument           = 0x080000,
        FunctionModifiers        = 0xff0000,

        // Argument modifier tags (0x01000000 ... 0xf0000000)
        ConversionRule           = 0x01000000,
        ReplaceType              = 0x02000000,
        ReplaceDefaultExpression = 0x04000000,
        RemoveArgument           = 0x08000000,
        DefineOwnership          = 0x10000000,
        RemoveDefaultExpression  = 0x20000000,
        NoNullPointers           = 0x40000000,
        ReferenceCount           = 0x80000000,
        ArgumentModifiers        = 0xff000000
    };

    StackElement(StackElement *p) : entry(0), type(None), parent(p){ }

    TypeEntry *entry;
    ElementType type;
    StackElement *parent;

    union {
        TemplateInstance *templateInstance;
        TemplateEntry *templateEntry;
        CustomFunction *customFunction;
    } value;
};


TypeDatabase *TypeDatabase::instance()
{
    static TypeDatabase *db = new TypeDatabase();
    return db;
}

TypeDatabase::TypeDatabase() : m_suppressWarnings(true)
{
    addType(new StringTypeEntry("QString"));

    StringTypeEntry *e = new StringTypeEntry("QLatin1String");
    e->setPreferredConversion(false);
    addType(e);

    e = new StringTypeEntry("QStringRef");
    e->setPreferredConversion(false);
    addType(e);

    e = new StringTypeEntry("QXmlStreamStringRef");
    e->setPreferredConversion(false);
    addType(e);

    addType(new CharTypeEntry("QChar"));

    CharTypeEntry *c = new CharTypeEntry("QLatin1Char");
    c->setPreferredConversion(false);
    addType(c);

    {
        VariantTypeEntry *qvariant = new VariantTypeEntry("QVariant");
        qvariant->setCodeGeneration(TypeEntry::GenerateNothing);
        addType(qvariant);
    }

    {
        JObjectWrapperTypeEntry *wrapper = new JObjectWrapperTypeEntry("JObjectWrapper");
        wrapper->setCodeGeneration(TypeEntry::GenerateNothing);
        addType(wrapper);
    }

    //addType(new ThreadTypeEntry());
    addType(new VoidTypeEntry());

    // Predefined containers...
    addType(new ContainerTypeEntry("QList", ContainerTypeEntry::ListContainer));
    addType(new ContainerTypeEntry("QStringList", ContainerTypeEntry::StringListContainer));
    addType(new ContainerTypeEntry("QLinkedList", ContainerTypeEntry::LinkedListContainer));
    addType(new ContainerTypeEntry("QVector", ContainerTypeEntry::VectorContainer));
    addType(new ContainerTypeEntry("QStack", ContainerTypeEntry::StackContainer));
    addType(new ContainerTypeEntry("QSet", ContainerTypeEntry::SetContainer));
    addType(new ContainerTypeEntry("QMap", ContainerTypeEntry::MapContainer));
    addType(new ContainerTypeEntry("QHash", ContainerTypeEntry::HashContainer));
    addType(new ContainerTypeEntry("QPair", ContainerTypeEntry::PairContainer));
    addType(new ContainerTypeEntry("QQueue", ContainerTypeEntry::QueueContainer));
    addType(new ContainerTypeEntry("QMultiMap", ContainerTypeEntry::MultiMapContainer));

    addRemoveFunctionToTemplates(this);
}

bool TypeDatabase::parseFile(const QString &filename, bool generate)
{
    QFile file(filename);
    Q_ASSERT(file.exists());

    int count = m_entries.size();

    XMLParser parser(filename, generate);
    bool ok = parser.parse();
    // QXmlInputSource source(&file);
    // QXmlSimpleReader reader;
    // Handler handler(this, generate);

    // reader.setContentHandler(&handler);
    // reader.setErrorHandler(&handler);

    // bool ok = reader.parse(&source, false);

    int newCount = m_entries.size();

    ReportHandler::debugSparse(QString::fromLatin1("Parsed: '%1', %2 new entries")
                               .arg(filename)
                               .arg(newCount - count));

    return ok;
}

QString PrimitiveTypeEntry::javaObjectName() const
{
    static QHash<QString, QString> table;
    if (table.isEmpty()) {
        table["boolean"] = "Boolean";
        table["byte"] = "Byte";
        table["char"] = "Character";
        table["short"] = "Short";
        table["int"] = "Integer";
        table["long"] = "Long";
        table["float"] = "Float";
        table["double"] = "Double";
    }
    Q_ASSERT(table.contains(targetLangName()));
    return table[targetLangName()];
}

ContainerTypeEntry *TypeDatabase::findContainerType(const QString &name)
{
    QString template_name = name;

    int pos = name.indexOf('<');
    if (pos > 0)
        template_name = name.left(pos);

    TypeEntry *type_entry = findType(template_name);
    if (type_entry && type_entry->isContainer())
        return static_cast<ContainerTypeEntry *>(type_entry);
    return 0;
}

PrimitiveTypeEntry *TypeDatabase::findTargetLangPrimitiveType(const QString &java_name)
{
    foreach (QList<TypeEntry *> entries, m_entries.values()) {
        foreach (TypeEntry *e, entries) {
            if (e && e->isPrimitive()) {
                PrimitiveTypeEntry *pe = static_cast<PrimitiveTypeEntry *>(e);
                if (pe->targetLangName() == java_name && pe->preferredConversion())
                    return pe;
            }
        }
    }

    return 0;
}

IncludeList TypeDatabase::extraIncludes(const QString &className)
{
    ComplexTypeEntry *typeEntry = findComplexType(className);
    if (typeEntry != 0)
        return typeEntry->extraIncludes();
    else
        return IncludeList();
}



QString Include::toString() const
{
    if (type == IncludePath)
        return "#include <" + name + '>';
    else if (type == LocalPath)
        return "#include \"" + name + "\"";
    else
        return "import " + name + ";";
}

QString Modification::accessModifierString() const
{
    if (isPrivate()) return "private";
    if (isProtected()) return "protected";
    if (isPublic()) return "public";
    if (isFriendly()) return "friendly";
    return QString();
}

FunctionModificationList ComplexTypeEntry::functionModifications(const QString &signature) const
{
    FunctionModificationList lst;
    for (int i=0; i<m_function_mods.count(); ++i) {
        FunctionModification mod = m_function_mods.at(i);
        if (mod.signature == signature) {
            lst << mod;
        }
    }

    return lst;
}

FieldModification ComplexTypeEntry::fieldModification(const QString &name) const
{
    for (int i=0; i<m_field_mods.size(); ++i)
        if (m_field_mods.at(i).name == name)
            return m_field_mods.at(i);
    FieldModification mod;
    mod.name = name;
    mod.modifiers = FieldModification::Readable | FieldModification::Writable;
    return mod;
}

QString ContainerTypeEntry::javaPackage() const
{
    if (m_type == PairContainer)
        return "com.trolltech.qt";
    return "java.util";
}

QString ContainerTypeEntry::targetLangName() const
{

    switch (m_type) {
    case StringListContainer: return "List";
    case ListContainer: return "List";
    case LinkedListContainer: return "LinkedList";
    case VectorContainer: return "List";
    case StackContainer: return "Stack";
    case QueueContainer: return "Queue";
    case SetContainer: return "Set";
    case MapContainer: return "SortedMap";
    case MultiMapContainer: return "SortedMap";
    case HashContainer: return "HashMap";
        //     case MultiHashCollectio: return "MultiHash";
    case PairContainer: return "QPair";
    default:
        qWarning("bad type... %d", m_type);
        break;
    }
    return QString();
}

QString ContainerTypeEntry::qualifiedCppName() const
{
    if (m_type == StringListContainer)
        return "QStringList";
    return ComplexTypeEntry::qualifiedCppName();
}

QString EnumTypeEntry::javaQualifier() const
{
    TypeEntry *te = TypeDatabase::instance()->findType(m_qualifier);
    if (te != 0)
        return te->targetLangName();
    else
        return m_qualifier;
}

QString EnumTypeEntry::jniName() const
{
    return "jint";
}

QString FlagsTypeEntry::jniName() const
{
    return "jint";
}

void EnumTypeEntry::addEnumValueRedirection(const QString &rejected, const QString &usedValue)
{
    m_enum_redirections << EnumValueRedirection(rejected, usedValue);
}

QString EnumTypeEntry::enumValueRedirection(const QString &value) const
{
    for (int i=0; i<m_enum_redirections.size(); ++i)
        if (m_enum_redirections.at(i).rejected == value)
            return m_enum_redirections.at(i).used;
    return QString();
}

QString FlagsTypeEntry::qualifiedTargetLangName() const
{
    return javaPackage() + "." + m_enum->javaQualifier() + "." + targetLangName();
}


void TypeDatabase::addRejection(const QString &class_name, const QString &function_name,
                                const QString &field_name, const QString &enum_name)
{
    TypeRejection r;
    r.class_name = class_name;
    r.function_name = function_name;
    r.field_name = field_name;
    r.enum_name = enum_name;

    m_rejections << r;
}

bool TypeDatabase::isClassRejected(const QString &class_name)
{
    if (!m_rebuild_classes.isEmpty())
        return !m_rebuild_classes.contains(class_name);

    foreach (const TypeRejection &r, m_rejections)
        if (r.class_name == class_name && r.function_name == "*" && r.field_name == "*" && r.enum_name == "*") {
            return true;
        }
    return false;
}

bool TypeDatabase::isEnumRejected(const QString &class_name, const QString &enum_name)
{
    foreach (const TypeRejection &r, m_rejections) {
        if (r.enum_name == enum_name
            && (r.class_name == class_name || r.class_name == "*")) {
            return true;
        }
    }

    return false;
}

bool TypeDatabase::isFunctionRejected(const QString &class_name, const QString &function_name)
{
    foreach (const TypeRejection &r, m_rejections)
        if (r.function_name == function_name &&
            (r.class_name == class_name || r.class_name == "*"))
            return true;
    return false;
}


bool TypeDatabase::isFieldRejected(const QString &class_name, const QString &field_name)
{
    foreach (const TypeRejection &r, m_rejections)
        if (r.field_name == field_name &&
            (r.class_name == class_name || r.class_name == "*"))
            return true;
    return false;
}

FlagsTypeEntry *TypeDatabase::findFlagsType(const QString &name) const
{
    FlagsTypeEntry *fte = (FlagsTypeEntry *) findType(name);
    return fte ? fte : (FlagsTypeEntry *) m_flags_entries.value(name);
}

QString TypeDatabase::globalNamespaceClassName(const TypeEntry * /*entry*/) {
    return QLatin1String("Global");
}


/*!
 * The Visual Studio 2002 compiler doesn't support these symbols,
 * which our typedefs unforntuatly expand to.
 */
QString fixCppTypeName(const QString &name)
{
    if (name == "long long") return "qint64";
    else if (name == "unsigned long long") return "quint64";
    return name;
}

QString formattedCodeHelper(QTextStream &s, Indentor &indentor, QStringList &lines) {
    bool multilineComment = false;
    bool lastEmpty = true;
    QString lastLine;
    while (!lines.isEmpty()) {
        const QString line = lines.takeFirst().trimmed();
        if (line.isEmpty()) {
            if (!lastEmpty)
                s << Qt::endl;
            lastEmpty = true;
            continue;
        } else {
            lastEmpty = false;
        }
        if (line.startsWith("/*"))
            multilineComment = true;

        if (multilineComment) {
            s << indentor;
            if (line.startsWith("*"))
                s << " ";
            s << line << Qt::endl;
            if (line.endsWith("*/"))
                multilineComment = false;
        } else if (line.startsWith("}")) {
            return line;
        } else if (line.endsWith("}")) {
            s << indentor << line << Qt::endl;
            return 0;
        } else if(line.endsWith("{")) {
            s << indentor << line << Qt::endl;
            QString tmp;
            {
                Indentation indent(indentor);
                tmp = formattedCodeHelper(s, indentor, lines);
            }
            if (!tmp.isNull()) {
                s << indentor << tmp << Qt::endl;
            }
            lastLine = tmp;
            continue;
        } else {
            s << indentor;
            if (!lastLine.isEmpty() &&
                !lastLine.endsWith(";") &&
                !line.startsWith("@") &&
                !line.startsWith("//") &&
                !lastLine.startsWith("//") &&
                !lastLine.endsWith("}") &&
                !line.startsWith("{"))
                s << "    ";
            s << line << Qt::endl;
        }
        lastLine = line;
    }
    return 0;
}


QTextStream &CodeSnip::formattedCode(QTextStream &s, Indentor &indentor) const
{
    QStringList lst(code().split("\n"));
    while (!lst.isEmpty()) {
        QString tmp = formattedCodeHelper(s, indentor, lst);
        if (!tmp.isNull()) {
            s << indentor << tmp << Qt::endl;
        }
    }
    s.flush();
    return s;
}

QString TemplateInstance::expandCode() const{
    TemplateEntry *templateEntry = TypeDatabase::instance()->findTemplate(m_name);
    if(templateEntry){
        QString res = templateEntry->code();
        foreach(QString key, replaceRules.keys()){
            res.replace(key, replaceRules[key]);
        }
        return "// TEMPLATE - " + m_name + " - START" + res + "// TEMPLATE - " + m_name + " - END";
    }
    else{
        ReportHandler::warning("insert-template referring to non-existing template '" + m_name + "'");
    }
    return QString();
}


QString CodeSnipAbstract::code() const{
    QString res;
    foreach(CodeSnipFragment *codeFrag, codeList){
        res.append(codeFrag->code());
    }
    return res;
}

QString CodeSnipFragment::code() const{
    if(m_instance)
        return m_instance->expandCode();
    else
        return m_code;
}

QString FunctionModification::toString() const
{
    QString str = signature + QLatin1String("->");
    if (modifiers & AccessModifierMask) {
        switch (modifiers & AccessModifierMask) {
        case Private: str += QLatin1String("private"); break;
        case Protected: str += QLatin1String("protected"); break;
        case Public: str += QLatin1String("public"); break;
        case Friendly: str += QLatin1String("friendly"); break;
        }
    }

    if (modifiers & Final) str += QLatin1String("final");
    if (modifiers & NonFinal) str += QLatin1String("non-final");

    if (modifiers & Readable) str += QLatin1String("readable");
    if (modifiers & Writable) str += QLatin1String("writable");

    if (modifiers & CodeInjection) {
        foreach (CodeSnip s, snips) {
            str += QLatin1String("\n//code injection:\n");
            str += s.code();
        }
    }

    if (modifiers & Rename) str += QLatin1String("renamed:") + renamedToName;

    if (modifiers & Deprecated) str += QLatin1String("deprecate");

    if (modifiers & ReplaceExpression) str += QLatin1String("replace-expression");

    return str;
}

static void removeFunction(ComplexTypeEntry *e, const char *signature)
{
    FunctionModification mod;
    mod.signature = TypeSystem::normalizedSignature(signature);
    mod.removal = TypeSystem::All;

    e->addFunctionModification(mod);
}




static void injectCode(ComplexTypeEntry *e,
                       const char *signature,
                       const QByteArray &code,
                       const ArgumentMap &args)
{
    CodeSnip snip;
    snip.language = TypeSystem::NativeCode;
    snip.position = CodeSnip::Beginning;
    snip.addCode(QString::fromLatin1(code));
    snip.argumentMap = args;

    FunctionModification mod;
    mod.signature = TypeSystem::normalizedSignature(signature);
    mod.snips << snip;
    mod.modifiers = Modification::CodeInjection;
    e->addFunctionModification(mod);
}


static void addRemoveFunctionToTemplates(TypeDatabase *db)
{
    ContainerTypeEntry *qvector = db->findContainerType(QLatin1String("QVector"));
    removeFunction(qvector, "constData() const");
    removeFunction(qvector, "data() const");
    removeFunction(qvector, "data()");
    removeFunction(qvector, "first()");
    removeFunction(qvector, "last()");
    removeFunction(qvector, "operator[](int)");
    removeFunction(qvector, "operator[](int) const");
    removeFunction(qvector, "operator=(QVector<T>)");

    ContainerTypeEntry *qlist = db->findContainerType(QLatin1String("QList"));
    removeFunction(qlist, "constData() const");
    removeFunction(qlist, "data() const");
    removeFunction(qlist, "data()");
    removeFunction(qlist, "back()");
    removeFunction(qlist, "front()");
    removeFunction(qlist, "first()");
    removeFunction(qlist, "last()");
    removeFunction(qlist, "operator[](int)");
    removeFunction(qlist, "operator[](int) const");
    removeFunction(qlist, "operator=(QList<T>)");

    ContainerTypeEntry *qqueue = db->findContainerType(QLatin1String("QQueue"));
    removeFunction(qqueue, "head() const");

    // QtScript: The next part is Java-specific, skip it for now...
    return;

    ArgumentMap args1;
    args1[1] = QLatin1String("$1");
    ArgumentMap args2 = args1;
    args2[2] = QLatin1String("$2");

    QByteArray code =
        "\nif ($1 >= __qt_this->size() || $1 < 0) {"
        "\n   __jni_env->ThrowNew(__jni_env->FindClass(\"java/lang/IndexOutOfBoundsException\"),"
        "\n                       QString::fromLatin1(\"Accessing container of size %3 at %4\")"
        "\n                       .arg(__qt_this->size()).arg($1).toLatin1());"
        "\n   return;"
        "\n}";

    QByteArray code_with_return = QByteArray(code).replace("return;", "return 0;");

    QByteArray code_index_length =
        "\nif ($1 < 0 || $2 < 0 || ($1 + $2) >= __qt_this->size()) {"
        "\n   __jni_env->ThrowNew(__jni_env->FindClass(\"java/lang/IndexOutOfBoundsException\"),"
        "\n                       QString::fromLatin1(\"Accessing container of size %3 from %4 to %5\")"
        "\n                       .arg(__qt_this->size()).arg($1).arg($1+$2).toLatin1());"
        "\n   return;"
        "\n}";

    QByteArray code_non_empty =
        "\nif (__qt_this->isEmpty()) {"
        "\n   __jni_env->ThrowNew(__jni_env->FindClass(\"java/lang/IndexOutOfBoundsException\"),"
        "\n                       QString::fromLatin1(\"Accessing empty container...\").toLatin1());"
        "\n   return;"
        "\n}";

    QByteArray code_two_indices =
        "\nif ($1 < 0 || $2 < 0 || $1 >= __qt_this->size() || $2 >= __qt_this->size()) {"
        "\n   __jni_env->ThrowNew(__jni_env->FindClass(\"java/lang/IndexOutOfBoundsException\"),"
        "\n                       QString::fromLatin1(\"Accessing container of size %3 from %4 to %5\")"
        "\n                       .arg(__qt_this->size()).arg($1).arg($1+$2).toLatin1());"
        "\n   return;"
        "\n}";

    { // QVector safty...
        injectCode(qvector, "at(int) const", code_with_return, args1);
        injectCode(qvector, "replace(int,T)", code, args1);
        injectCode(qvector, "remove(int)", code, args1);
        injectCode(qvector, "remove(int, int)", code_index_length, args2);
        injectCode(qvector, "pop_back()", code_non_empty, ArgumentMap());
        injectCode(qvector, "pop_front()", code_non_empty, ArgumentMap());
    }

    { // QList safty...
        injectCode(qlist, "at(int) const", code_with_return, args1);
        injectCode(qlist, "replace(int, T)", code, args1);
        injectCode(qlist, "pop_back()", code_non_empty, ArgumentMap());
        injectCode(qlist, "pop_front()", code_non_empty, ArgumentMap());
        injectCode(qlist, "swap(int, int)", code_two_indices, args2);
        injectCode(qlist, "move(int, int)", code_two_indices, args2);
        injectCode(qlist, "removeAt(int)", code, args1);
        injectCode(qlist, "takeAt(int)", code_with_return, args1);
    }

}

QByteArray TypeSystem::normalizedSignature(const char* signature)
{
  QByteArray result = QMetaObject::normalizedSignature(signature);
  result.replace("unsigned ", "u");
  result.replace("qreal", "double");
  result.replace("long long", "longlong");
  result.replace("qlonglong", "longlong");
  result.replace("qulonglong", "ulonglong");
  result.replace("qint64", "longlong");
  result.replace("quint64", "ulonglong");
  result.replace("QStringList<QString>", "QStringList");
  return result;
}