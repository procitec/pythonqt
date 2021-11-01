/*
 *
 *  Copyright (C) 2010 MeVis Medical Solutions AG All Rights Reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  Further, this software is distributed without any warranty that it is
 *  free of the rightful claim of any third person regarding infringement
 *  or the like.  Any license provided herein, whether implied or
 *  otherwise, applies only to this software file.  Patent licenses, if
 *  any, provided herein do not apply to combinations of this program with
 *  other software, or any other product whatsoever.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact information: MeVis Medical Solutions AG, Universitaetsallee 29,
 *  28359 Bremen, Germany or:
 *
 *  http://www.mevis.de
 *
 */

//----------------------------------------------------------------------------------
/*!
// \file    PythonQtTests.cpp
// \author  Florian Link
// \author  Last changed by $Author: florian $
// \date    2006-05
*/
//----------------------------------------------------------------------------------

#include "PythonQtTests.h"

void PythonQtTestSlotCalling::initTestCase()
{
  _helper = new PythonQtTestSlotCallingHelper(this);
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  main.evalScript("import PythonQt");
  PythonQt::self()->addObject(main, "obj", _helper);
}

void PythonQtTestSlotCalling::init() {

}


void* polymorphic_ClassB_Handler(const void* ptr, const char** className) {
  ClassB* o = (ClassB*)ptr;
  if (o->type()==2) {
    *className = "ClassB";
    return (ClassB*)o;
  }
  if (o->type()==3) {
    *className = "ClassC";
    return (ClassC*)o;
  }
  if (o->type()==4) {
    *className = "ClassD";
    return (ClassD*)o;
  }
  return NULL;
}

void PythonQtTestSlotCalling::testInheritance() {
  PythonQt::self()->registerCPPClass("ClassA",NULL,NULL, PythonQtCreateObject<ClassAWrapper>);
  PythonQt::self()->registerCPPClass("ClassB",NULL,NULL, PythonQtCreateObject<ClassBWrapper>);
  PythonQt::self()->registerCPPClass("ClassC",NULL,NULL, PythonQtCreateObject<ClassCWrapper>);
  PythonQt::self()->addParentClass("ClassC", "ClassA", PythonQtUpcastingOffset<ClassC,ClassA>());
  PythonQt::self()->addParentClass("ClassC", "ClassB", PythonQtUpcastingOffset<ClassC,ClassB>());
  PythonQt::self()->registerClass(&ClassD::staticMetaObject, NULL, PythonQtCreateObject<ClassDWrapper>);
  PythonQt::self()->addParentClass("ClassD", "ClassA", PythonQtUpcastingOffset<ClassD,ClassA>());
  PythonQt::self()->addParentClass("ClassD", "ClassB", PythonQtUpcastingOffset<ClassD,ClassB>());

  PythonQtObjectPtr classA = PythonQt::self()->getMainModule().getVariable("PythonQt.private.ClassA");
  PythonQtObjectPtr classB = PythonQt::self()->getMainModule().getVariable("PythonQt.private.ClassB");
  PythonQtObjectPtr classC = PythonQt::self()->getMainModule().getVariable("PythonQt.private.ClassC");
  PythonQtObjectPtr classD = PythonQt::self()->getMainModule().getVariable("PythonQt.private.ClassD");
  QVERIFY(classA);
  QVERIFY(classB);
  QVERIFY(classC);
  QVERIFY(classD);

  QVERIFY(_helper->runScript("a = PythonQt.private.ClassA();\nif obj.getClassAPtr(a).getX()==1: obj.setPassed();\n"));
  QEXPECT_FAIL("", "ClassB can not be converted to ClassA", Continue);
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassB();\nif obj.getClassAPtr(a).getX()==1: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassC();\nif obj.getClassAPtr(a).getX()==1: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassD();\nif obj.getClassAPtr(a).getX()==1: obj.setPassed();\n"));

  QEXPECT_FAIL("", "ClassA can not be converted to ClassB", Continue);
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassA();\nif obj.getClassBPtr(a).getY()==2: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassB();\nif obj.getClassBPtr(a).getY()==2: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassC();\nif obj.getClassBPtr(a).getY()==2: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassD();\nif obj.getClassBPtr(a).getY()==2: obj.setPassed();\n"));

  QEXPECT_FAIL("", "ClassA can not be converted to ClassC", Continue);
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassA();\nif obj.getClassCPtr(a).getX()==1: obj.setPassed();\n"));
  QEXPECT_FAIL("", "ClassB can not be converted to ClassC", Continue);
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassB();\nif obj.getClassCPtr(a).getX()==1: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassC();\nif obj.getClassCPtr(a).getX()==1: obj.setPassed();\n"));
  QEXPECT_FAIL("", "ClassD can not be converted to ClassC", Continue);
  QVERIFY(_helper->runScript("a = PythonQt.private.ClassD();\nif obj.getClassCPtr(a).getX()==1: obj.setPassed();\n"));

  QVERIFY(_helper->runScript("if type(obj.createClassA())==PythonQt.private.ClassA: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassB())==PythonQt.private.ClassB: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassCAsA())==PythonQt.private.ClassA: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassCAsB())==PythonQt.private.ClassB: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassD())==PythonQt.private.ClassD: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassDAsA())==PythonQt.private.ClassA: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassDAsB())==PythonQt.private.ClassB: obj.setPassed();\n"));

  PythonQt::self()->addPolymorphicHandler("ClassB", polymorphic_ClassB_Handler);

  QVERIFY(_helper->runScript("if type(obj.getClassBPtr(obj.createClassB()))==PythonQt.private.ClassB: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassCAsB())==PythonQt.private.ClassC: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if type(obj.createClassDAsB())==PythonQt.private.ClassD: obj.setPassed();\n"));

}

void PythonQtTestSlotCalling::testNoArgSlotCall()
{
  QVERIFY(_helper->runScript("obj.testNoArg(); obj.setPassed();\n"));
}

void PythonQtTestSlotCalling::testOverloadedCall()
{
  QVERIFY(_helper->runScript("obj.overload(False); obj.setPassed();\n", 0));
  QVERIFY(_helper->runScript("obj.overload(True); obj.setPassed();\n", 0));
  QVERIFY(_helper->runScript("obj.overload(12.5); obj.setPassed();\n", 1));
  QVERIFY(_helper->runScript("obj.overload(12); obj.setPassed();\n", 2));
  QVERIFY(_helper->runScript("obj.overload('test'); obj.setPassed();\n", 3));
  QVERIFY(_helper->runScript("obj.overload(u'test'); obj.setPassed();\n", 3));
  QVERIFY(_helper->runScript("obj.overload(('test','test2')); obj.setPassed();\n", 4));
  QVERIFY(_helper->runScript("obj.overload(obj); obj.setPassed();\n", 5));
  QVERIFY(_helper->runScript("obj.overload(12,13); obj.setPassed();\n", 6));
}


void PythonQtTestSlotCalling::testKeywordCall()
{
  QVERIFY(_helper->runScript("if obj.keywordInt(5,value=6)==11: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.keywordOnly(value=6)==1: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.keywordOnly(arg1='test1',arg2='test2')==2: obj.setPassed();\n"));
}

void PythonQtTestSlotCalling::testPyObjectSlotCall()
{
  QVERIFY(_helper->runScript("if obj.getPyObject(PythonQt)==PythonQt: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getPyObject('Hello')=='Hello': obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getPyObjectFromVariant(PythonQt)==PythonQt: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getPyObjectFromVariant2(PythonQt)==PythonQt: obj.setPassed();\n"));
}

void PythonQtTestSlotCalling::testPODSlotCalls()
{
  QVERIFY(_helper->runScript("if obj.getBool(False)==False: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getBool(True)==True: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getInt(-42)==-42: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getUInt(42)==42: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getShort(-43)==-43: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getUShort(43)==43: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getChar(-12)==-12: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getUChar(12)==12: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getLong(-256*256*256)==-256*256*256: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getULong(256*256*256)==256*256*256: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getLongLong(-42)==-42: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getULongLong(42)==42: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getQChar(4096)==4096: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getDouble(47.12)==47.12: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if abs(obj.getFloat(47.11)-47.11)<0.01: obj.setPassed();\n"));
}


void PythonQtTestSlotCalling::testObjectSlotCalls()
{
  QVERIFY(_helper->runScript("if obj.getQObject(obj)==obj: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getTestObject(obj)==obj: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.getNewObject().className()=='PythonQtTestSlotCallingHelper': obj.setPassed();\n"));
  QEXPECT_FAIL("", "Testing to pass a QObject when another object was expected", Continue);
  QVERIFY(_helper->runScript("if obj.getQWidget(obj)==obj: obj.setPassed();\n"));
}

void PythonQtTestSlotCalling::testCppFactory()
{
  PythonQtTestCppFactory* f = new PythonQtTestCppFactory;
  PythonQt::self()->addInstanceDecorators(new PQCppObjectDecorator);
  // do not register, since we want to know if that works as well
  //qRegisterMetaType<PQCppObjectNoWrap>("PQCppObjectNoWrap");
  PythonQt::self()->addDecorators(new PQCppObjectNoWrapDecorator);

  PythonQt::self()->addWrapperFactory(f);
  QVERIFY(_helper->runScript("if obj.createPQCppObject(12).getHeight()==12: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("if obj.createPQCppObject(12).getH()==12: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("pq1 = obj.createPQCppObject(12);\n"
    "pq2 = obj.createPQCppObject(13);\n"
    "pq3 = obj.getPQCppObject(pq1);\n"
    "pq4 = obj.getPQCppObject(pq2);\n"
    "if pq3.getHeight()==12 and pq4.getHeight()==13: obj.setPassed();\n"
    ));

  QVERIFY(_helper->runScript("if obj.createPQCppObjectNoWrap(12).getH()==12: obj.setPassed();\n"));
  
  QVERIFY(_helper->runScript("if obj.getPQCppObjectNoWrapAsValue().getH()==47: obj.setPassed();\n"));
  
  qRegisterMetaType<PQUnknownButRegisteredValueObject>("PQUnknownButRegisteredValueObject");
  QVERIFY(_helper->runScript("a = obj.getUnknownButRegisteredValueObjectAsPtr();print(a);\nif a!=None: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = obj.getUnknownButRegisteredValueObjectAsValue();print(a);\nif a!=None: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("a = obj.getUnknownValueObjectAsPtr();print(a);\nif a!=None: obj.setPassed();\n"));
  QEXPECT_FAIL("", "Testing by value return without the object being registered as QMetaType or having registered a default constructor decorator", Continue);
  QVERIFY(_helper->runScript("a = obj.getUnknownValueObjectAsValue();print(a);\nif a!=None: obj.setPassed();\n"));
  
  // expect to get strict call to double overload
  QVERIFY(_helper->runScript("obj.testNoArg()\nfrom PythonQt.private import PQCppObjectNoWrap\na = PQCppObjectNoWrap(22.2)\nif a.getH()==2: obj.setPassed();\n"));
  // expect to get un-strict call to double overload
  QVERIFY(_helper->runScript("obj.testNoArg()\nfrom PythonQt.private import PQCppObjectNoWrap\na = PQCppObjectNoWrap(22)\nif a.getH()==2: obj.setPassed();\n"));
  // expect to get strict call to copy constructor overload
  QVERIFY(_helper->runScript("obj.testNoArg()\nfrom PythonQt.private import PQCppObjectNoWrap\na = PQCppObjectNoWrap(PQCppObjectNoWrap())\nprint(a.getH())\nif a.getH()==1: obj.setPassed();\n"));

  // test decorated enums
  // already registered by signals test
  //PythonQt::self()->registerCPPClass("PQCppObject2",NULL,NULL, PythonQtCreateObject<PQCppObject2Decorator>);
  
  // local enum (decorated)
  QVERIFY(_helper->runScript("obj.testNoArg()\nfrom PythonQt.private import PQCppObject2\na = PQCppObject2()\nprint(a.testEnumFlag1)\nif a.testEnumFlag1(PQCppObject2.TestEnumValue2)==PQCppObject2.TestEnumValue2: obj.setPassed();\n"));
  // enum with namespace (decorated)
  QVERIFY(_helper->runScript("obj.testNoArg()\nfrom PythonQt.private import PQCppObject2\na = PQCppObject2()\nif a.testEnumFlag2(PQCppObject2.TestEnumValue2)==PQCppObject2.TestEnumValue2: obj.setPassed();\n"));
  // with int overload to check overloading
  QVERIFY(_helper->runScript("obj.testNoArg()\nfrom PythonQt.private import PQCppObject2\na = PQCppObject2()\nif a.testEnumFlag3(PQCppObject2.TestEnumValue2)==PQCppObject2.TestEnumValue2: obj.setPassed();\n"));

}

PQCppObject2Decorator::TestEnumFlag PQCppObject2Decorator::testEnumFlag1(PQCppObject2* /*obj*/, PQCppObject2Decorator::TestEnumFlag flag) {
  return flag;
}

PQCppObject2::TestEnumFlag PQCppObject2Decorator::testEnumFlag2(PQCppObject2* /*obj*/, PQCppObject2::TestEnumFlag flag) {
  return flag;
}

// with int overload
PQCppObject2Decorator::TestEnumFlag PQCppObject2Decorator::testEnumFlag3(PQCppObject2* /*obj*/, int /*flag*/) {
  return (TestEnumFlag)-1;
}
PQCppObject2Decorator::TestEnumFlag PQCppObject2Decorator::testEnumFlag3(PQCppObject2* /*obj*/, PQCppObject2Decorator::TestEnumFlag flag) {
  return flag;
}

void PythonQtTestSlotCalling::testProperties()
{
  QVERIFY(_helper->runScript("obj.intProp = 47\nif obj.intProp == 47: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.floatProp = 47\nif obj.floatProp == 47: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.variantListProp = (1,'test')\nif obj.variantListProp == (1,'test'): obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.variantMapProp = {'test':'a', 'bla':'blubb'}\nif obj.variantMapProp == {'test':'a', 'bla':'blubb'}: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.variantProp = {'test':'a', 'bla':'blubb'}\nif obj.variantProp == {'test':'a', 'bla':'blubb'}: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.variantProp = 'test'\nif obj.variantProp == 'test': obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.variantProp = 47.11\nif obj.variantProp == 47.11: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.qObjectProp = obj\nif obj.qObjectProp == obj: obj.setPassed();\n"));
  QVERIFY(_helper->runScript("obj.qObjectListProp = (obj,obj,obj)\nif obj.qObjectListProp == (obj,obj,obj): obj.setPassed();\n"));
}

bool PythonQtTestSlotCallingHelper::runScript(const char* script, int expectedOverload)
{
  _called = false;
  _passed = false;
  _calledOverload = -1;
  PyRun_SimpleString(script);
  return _called && _passed && _calledOverload==expectedOverload;
}


void PythonQtTestSignalHandler::initTestCase()
{
  _helper = new PythonQtTestSignalHandlerHelper(this);
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  PythonQt::self()->addObject(main, "obj", _helper);
}

void PythonQtTestSignalHandler::testSignalHandler()
{
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  PyRun_SimpleString("def testIntSignal(a):\n  if a==12: obj.setPassed();\n");
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(intSignal(int)), main, "testIntSignal"));
  QVERIFY(_helper->emitIntSignal(12));

  PyRun_SimpleString("def testFloatSignal(a):\n  if a==12: obj.setPassed();\n");
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(floatSignal(float)), main, "testFloatSignal"));
  QVERIFY(_helper->emitFloatSignal(12));

  // test decorated enums
  PythonQt::self()->registerCPPClass("PQCppObject2",NULL,NULL, PythonQtCreateObject<PQCppObject2Decorator>);

  PyRun_SimpleString("def testEnumSignal(a):\n  if a==1: obj.setPassed();\n");
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(enumSignal(PQCppObject2::TestEnumFlag)), main, "testEnumSignal"));
  QVERIFY(_helper->emitEnumSignal(PQCppObject2::TestEnumValue2));
  
  PyRun_SimpleString("def testVariantSignal(a):\n  if a==obj.expectedVariant(): obj.setPassed();\n");
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(variantSignal(QVariant)), main, "testVariantSignal"));
  _helper->setExpectedVariant(QString("Test"));
  QVERIFY(_helper->emitVariantSignal(QString("Test")));
  _helper->setExpectedVariant(12);
  QVERIFY(_helper->emitVariantSignal(12));
  _helper->setExpectedVariant(QStringList() << "test1" << "test2");
  QVERIFY(_helper->emitVariantSignal(QStringList() << "test1" << "test2"));
  _helper->setExpectedVariant(QVariant::fromValue((QObject*)_helper));
  QVERIFY(_helper->emitVariantSignal(QVariant::fromValue((QObject*)_helper)));

  PyRun_SimpleString("def testComplexSignal(a,b,l,o):\n  if a==12 and b==13 and l==('test1','test2') and o == obj: obj.setPassed();\n");
  // intentionally not normalized signal:
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(complexSignal( int, float , const QStringList  , QObject*)), main, "testComplexSignal"));
  QVERIFY(_helper->emitComplexSignal(12,13,QStringList() << "test1" << "test2", _helper));

  // try removing the handler
  QVERIFY(PythonQt::self()->removeSignalHandler(_helper, SIGNAL(complexSignal( int, float , const QStringList  , QObject*)), main, "testComplexSignal"));
  // and emit the signal, which should fail because the handler was removed
  QVERIFY(!_helper->emitComplexSignal(12,13,QStringList() << "test1" << "test2", _helper));

  QVERIFY(PythonQt::self()->removeSignalHandler(_helper, SIGNAL(intSignal(int)), main, "testIntSignal"));
  QVERIFY(PythonQt::self()->removeSignalHandler(_helper, SIGNAL(floatSignal(float)), main, "testFloatSignal"));
  QVERIFY(PythonQt::self()->removeSignalHandler(_helper, SIGNAL(variantSignal(QVariant)), main, "testVariantSignal"));
  QVERIFY(PythonQt::self()->removeSignalHandler(_helper, SIGNAL(enumSignal(PQCppObject2::TestEnumFlag)), main, "testEnumSignal"));

}

void PythonQtTestSignalHandler::testRecursiveSignalHandler()
{
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
  PyRun_SimpleString("def testSignal1(a):\n  obj.emitSignal2(a);\n");
  PyRun_SimpleString("def testSignal2(a):\n  obj.emitSignal3(float(a));\n");
  PyRun_SimpleString("def testSignal3(a):\n  if a==12: obj.setPassed();\n");
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(signal1(int)), main, "testSignal1"));
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(signal2(const QString&)), main, "testSignal2"));
  QVERIFY(PythonQt::self()->addSignalHandler(_helper, SIGNAL(signal3(float)), main, "testSignal3"));
  QVERIFY(_helper->emitSignal1(12));
}


void PythonQtTestApi::initTestCase()
{
  _helper = new PythonQtTestApiHelper();
  _main = PythonQt::self()->getMainModule();
  _main.evalScript("import PythonQt");
  _main.addObject("obj", _helper);
}

void PythonQtTestApi::testProperties()
{
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();
#ifdef PYTHONQT_SUPPORT_NAME_PROPERTY
  // check for name alias (for backward comp to Qt3)
  main.evalScript("obj.name = 'hello'");
  QVERIFY(QString("hello") == main.getVariable("obj.name").toString());
#endif

  main.evalScript("obj.objectName = 'hello2'");
  QVERIFY(QString("hello2") == main.getVariable("obj.objectName").toString());

}

void PythonQtTestApi::testDynamicProperties()
{
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();

  // this fails and should fail, but how could that be tested?
  // main.evalScript("obj.testProp = 1");
  
  // create a new dynamic property
  main.evalScript("obj.setProperty('testProp','testValue')");

  // read the property
  QVERIFY(QString("testValue") == main.getVariable("obj.testProp").toString());
  // modify and read again
  main.evalScript("obj.testProp = 12");
  QVERIFY(12 == main.getVariable("obj.testProp").toInt());

  // check if dynamic property is in dict
  QVERIFY(12 == main.evalScript("obj.__dict__['testProp']", Py_eval_input).toInt());

  // check if dynamic property is in introspection
  QStringList l = PythonQt::self()->introspection(PythonQt::self()->getMainModule(), "obj", PythonQt::Anything);
  QVERIFY(l.contains("testProp"));
  
  // check with None, previous value expected
  main.evalScript("obj.testProp = None");
  QVERIFY(12 == main.getVariable("obj.testProp").toInt());

  // remove the dynamic property
  main.evalScript("obj.setProperty('testProp', None)");

  // check if dynamic property is really gone
  QStringList l2 = PythonQt::self()->introspection(PythonQt::self()->getMainModule(), "obj", PythonQt::Anything);
  QVERIFY(!l2.contains("testProp"));
  
}


bool PythonQtTestApiHelper::call(const QString& function, const QVariantList& args, const QVariant& expectedResult) {
  _passed = false;
  QVariant r = PythonQt::self()->call(PythonQt::self()->getMainModule(), function, args);
  return _passed && expectedResult==r;
}

void PythonQtTestApi::testCall()
{
  PythonQtObjectPtr main = PythonQt::self()->getMainModule();

  QVERIFY(qvariant_cast<QObject*>(PythonQt::self()->getVariable(main, "obj"))==_helper);

  PyRun_SimpleString("def testCallNoArgs():\n  obj.setPassed();\n");
  QVERIFY(_helper->call("testCallNoArgs", QVariantList(), QVariant()));

  PyRun_SimpleString("def testCall1(a):\n if a=='test': obj.setPassed();\n return 'test2';\n");
  QVERIFY(_helper->call("testCall1", QVariantList() << QVariant("test"), QVariant(QString("test2"))));

  PyRun_SimpleString("def testCall2(a, b):\n if a=='test' and b==obj: obj.setPassed();\n return obj;\n");
  QVariant r = PythonQt::self()->call(PythonQt::self()->getMainModule(), "testCall2", QVariantList() << QVariant("test") << QVariant::fromValue((QObject*)_helper));
  QObject* p = qvariant_cast<QObject*>(r);
  QVERIFY(p==_helper);
}

void PythonQtTestApi::testVariables()
{
  PythonQt::self()->addObject(PythonQt::self()->getMainModule(), "someObject", _helper);
  QVariant v = PythonQt::self()->getVariable(PythonQt::self()->getMainModule(), "someObject");
  QObject* p = qvariant_cast<QObject*>(v);
  QVERIFY(p==_helper);
  // test for unset variable
  QVariant v2 = PythonQt::self()->getVariable(PythonQt::self()->getMainModule(), "someObject2");
  QVERIFY(v2==QVariant());

  QStringList l = PythonQt::self()->introspection(PythonQt::self()->getMainModule(), QString(), PythonQt::Variable);
  QSet<QString> s;
  // check that at least these three variables are set
  s << "obj" << "someObject";
  Q_FOREACH (QString value, s) {
    QVERIFY(l.indexOf(value)!=-1);
  }

  // insert a second time!
  PythonQt::self()->addObject(PythonQt::self()->getMainModule(), "someObject", _helper);
  // and remove
  PythonQt::self()->removeVariable(PythonQt::self()->getMainModule(), "someObject");
  // we expect to find no variable
  QVariant v4 = PythonQt::self()->getVariable(PythonQt::self()->getMainModule(), "someObject");
  QVERIFY(v4==QVariant());
}

void PythonQtTestApi::testImporter()
{
  PythonQt::self()->setImporter(_helper);
  PythonQt::self()->overwriteSysPath(QStringList() << "c:\\test");
  PyRun_SimpleString("import bla\n");
}

QByteArray PythonQtTestApiHelper::readFileAsBytes(const QString& /*filename*/)
{
  QByteArray b;
  return b;
}

QByteArray PythonQtTestApiHelper::readSourceFile(const QString& /*filename*/, bool& ok)
{
  QByteArray b;
  ok = true;
  return b;
}

bool PythonQtTestApiHelper::exists(const QString& /*filename*/)
{
  return true;
}

QDateTime PythonQtTestApiHelper::lastModifiedDate(const QString& /*filename*/) {
  return QDateTime::currentDateTime();
}


void PythonQtTestApi::testRedirect()
{
  connect(PythonQt::self(), SIGNAL(pythonStdOut(const QString&)), _helper, SLOT(stdOut(const QString&)));
  connect(PythonQt::self(), SIGNAL(pythonStdErr(const QString&)), _helper, SLOT(stdErr(const QString&)));
  PyRun_SimpleString("print('test')\n");
}

void PythonQtTestApiHelper::stdOut(const QString& s)
{
  qDebug() << s;
}

void PythonQtTestApiHelper::stdErr(const QString& s)
{
  qDebug() << s;
}

QObject* PythonQtTestCppFactory::create(const QByteArray& name, void *ptr)
{
  if (name == "PQCppObject") {
    return new PQCppObjectWrapper(ptr);
  }
  return NULL;
}
