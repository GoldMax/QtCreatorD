
try:
    import __builtin__
except:
    import builtins
try:
    import gdb
except:
    pass

import os
import os.path
import sys
import struct


def warn(message):
    print("XXX: %s\n" % message.encode("latin1"))

from dumper import *
from qttypes import *
from stdtypes import *
from misctypes import *
from boosttypes import *
from creatortypes import *
from dlang import *


#######################################################################
#
# Infrastructure
#
#######################################################################

def savePrint(output):
    try:
        print(output)
    except:
        out = ""
        for c in output:
            cc = ord(c)
            if cc > 127:
                out += "\\\\%d" % cc
            elif cc < 0:
                out += "\\\\%d" % (cc + 256)
            else:
                out += c
        print(out)

def registerCommand(name, func):

    class Command(gdb.Command):
        def __init__(self):
            super(Command, self).__init__(name, gdb.COMMAND_OBSCURE)
        def invoke(self, args, from_tty):
            savePrint(func(args))

    Command()


def listOfLocals(varList):
    frame = gdb.selected_frame()

    try:
        block = frame.block()
        #warn("BLOCK: %s " % block)
    except RuntimeError as error:
        #warn("BLOCK IN FRAME NOT ACCESSIBLE: %s" % error)
        return []
    except:
        warn("BLOCK NOT ACCESSIBLE FOR UNKNOWN REASONS")
        return []

    items = []
    shadowed = {}
    while True:
        if block is None:
            warn("UNEXPECTED 'None' BLOCK")
            break
        for symbol in block:
            name = symbol.print_name

            if name == "__in_chrg" or name == "__PRETTY_FUNCTION__":
                continue

            # "NotImplementedError: Symbol type not yet supported in
            # Python scripts."
            #warn("SYMBOL %s  (%s): " % (symbol, name))
            if name in shadowed:
                level = shadowed[name]
                name1 = "%s@%s" % (name, level)
                shadowed[name] = level + 1
            else:
                name1 = name
                shadowed[name] = 1
            #warn("SYMBOL %s  (%s, %s)): " % (symbol, name, symbol.name))
            item = LocalItem()
            item.iname = "local." + name1
            item.name = name1
            try:
                item.value = frame.read_var(name, block)
                #warn("READ 1: %s" % item.value)
                if not item.value.is_optimized_out:
                    #warn("ITEM 1: %s" % item.value)
                    items.append(item)
                    continue
            except:
                pass

            try:
                item.value = frame.read_var(name)
                #warn("READ 2: %s" % item.value)
                if not item.value.is_optimized_out:
                    #warn("ITEM 2: %s" % item.value)
                    items.append(item)
                    continue
            except:
                # RuntimeError: happens for
                #     void foo() { std::string s; std::wstring w; }
                # ValueError: happens for (as of 2010/11/4)
                #     a local struct as found e.g. in
                #     gcc sources in gcc.c, int execute()
                pass

            try:
                #warn("READ 3: %s %s" % (name, item.value))
                item.value = gdb.parse_and_eval(name)
                #warn("ITEM 3: %s" % item.value)
                items.append(item)
            except:
                # Can happen in inlined code (see last line of
                # RowPainter::paintChars(): "RuntimeError:
                # No symbol \"__val\" in current context.\n"
                pass

        # The outermost block in a function has the function member
        # FIXME: check whether this is guaranteed.
        if not block.function is None:
            break

        block = block.superblock

    return items



#######################################################################
#
# Types
#
#######################################################################

PointerCode = gdb.TYPE_CODE_PTR
ArrayCode = gdb.TYPE_CODE_ARRAY
StructCode = gdb.TYPE_CODE_STRUCT
UnionCode = gdb.TYPE_CODE_UNION
EnumCode = gdb.TYPE_CODE_ENUM
FlagsCode = gdb.TYPE_CODE_FLAGS
FunctionCode = gdb.TYPE_CODE_FUNC
IntCode = gdb.TYPE_CODE_INT
FloatCode = gdb.TYPE_CODE_FLT # Parts of GDB assume that this means complex.
VoidCode = gdb.TYPE_CODE_VOID
#SetCode = gdb.TYPE_CODE_SET
RangeCode = gdb.TYPE_CODE_RANGE
StringCode = gdb.TYPE_CODE_STRING
#BitStringCode = gdb.TYPE_CODE_BITSTRING
#ErrorTypeCode = gdb.TYPE_CODE_ERROR
MethodCode = gdb.TYPE_CODE_METHOD
MethodPointerCode = gdb.TYPE_CODE_METHODPTR
MemberPointerCode = gdb.TYPE_CODE_MEMBERPTR
ReferenceCode = gdb.TYPE_CODE_REF
CharCode = gdb.TYPE_CODE_CHAR
BoolCode = gdb.TYPE_CODE_BOOL
ComplexCode = gdb.TYPE_CODE_COMPLEX
TypedefCode = gdb.TYPE_CODE_TYPEDEF
NamespaceCode = gdb.TYPE_CODE_NAMESPACE
#Code = gdb.TYPE_CODE_DECFLOAT # Decimal floating point.
#Code = gdb.TYPE_CODE_MODULE # Fortran
#Code = gdb.TYPE_CODE_INTERNAL_FUNCTION


#######################################################################
#
# Convenience
#
#######################################################################

# Just convienience for 'python print ...'
class PPCommand(gdb.Command):
    def __init__(self):
        super(PPCommand, self).__init__("pp", gdb.COMMAND_OBSCURE)
    def invoke(self, args, from_tty):
        print(eval(args))

PPCommand()

# Just convienience for 'python print gdb.parse_and_eval(...)'
class PPPCommand(gdb.Command):
    def __init__(self):
        super(PPPCommand, self).__init__("ppp", gdb.COMMAND_OBSCURE)
    def invoke(self, args, from_tty):
        print(gdb.parse_and_eval(args))

PPPCommand()


def scanStack(p, n):
    p = int(p)
    r = []
    for i in xrange(n):
        f = gdb.parse_and_eval("{void*}%s" % p)
        m = gdb.execute("info symbol %s" % f, to_string=True)
        if not m.startswith("No symbol matches"):
            r.append(m)
        p += f.type.sizeof
    return r

class ScanStackCommand(gdb.Command):
    def __init__(self):
        super(ScanStackCommand, self).__init__("scanStack", gdb.COMMAND_OBSCURE)
    def invoke(self, args, from_tty):
        if len(args) == 0:
            args = 20
        savePrint(scanStack(gdb.parse_and_eval("$sp"), int(args)))

ScanStackCommand()


def bbsetup(args = ''):
    print(theDumper.bbsetup())

registerCommand("bbsetup", bbsetup)


#######################################################################
#
# Import plain gdb pretty printers
#
#######################################################################

class PlainDumper:
    def __init__(self, printer):
        self.printer = printer

    def __call__(self, d, value):
        printer = self.printer.invoke(value)
        lister = getattr(printer, "children", None)
        children = [] if lister is None else list(lister())
        d.putType(self.printer.name)
        val = printer.to_string().encode("hex")
        d.putValue(val, Hex2EncodedLatin1)
        d.putValue(printer.to_string())
        d.putNumChild(len(children))
        if d.isExpanded():
            with Children(d):
                for child in children:
                    d.putSubItem(child[0], child[1])

def importPlainDumpers(args):
    theDumper.importPlainDumpers()

registerCommand("importPlainDumpers", importPlainDumpers)



class OutputSafer:
    def __init__(self, d):
        self.d = d

    def __enter__(self):
        self.savedOutput = self.d.output
        self.d.output = []

    def __exit__(self, exType, exValue, exTraceBack):
        if self.d.passExceptions and not exType is None:
            showException("OUTPUTSAFER", exType, exValue, exTraceBack)
            self.d.output = self.savedOutput
        else:
            self.savedOutput.extend(self.d.output)
            self.d.output = self.savedOutput
        return False



#def couldBePointer(p, align):
#    type = lookupType("unsigned int")
#    ptr = gdb.Value(p).cast(type)
#    d = int(str(ptr))
#    warn("CHECKING : %s %d " % (p, ((d & 3) == 0 and (d > 1000 or d == 0))))
#    return (d & (align - 1)) and (d > 1000 or d == 0)


Value = gdb.Value



def stripTypedefs(typeobj):
    typeobj = typeobj.unqualified()
    while typeobj.code == TypedefCode:
        typeobj = typeobj.strip_typedefs().unqualified()
    return typeobj


#######################################################################
#
# LocalItem
#
#######################################################################

# Contains iname, name, and value.
class LocalItem:
    pass


#######################################################################
#
# Edit Command
#
#######################################################################

def bbedit(args):
    theDumper.bbedit(args)

registerCommand("bbedit", bbedit)


#######################################################################
#
# Frame Command
#
#######################################################################


def bb(args):
    try:
        print(theDumper.run(args))
    except:
        import traceback
        traceback.print_exc()

registerCommand("bb", bb)


#######################################################################
#
# The Dumper Class
#
#######################################################################


class Dumper(DumperBase):

    def __init__(self):
        DumperBase.__init__(self)

        # These values will be kept between calls to 'run'.
        self.isGdb = True
        self.childEventAddress = None
        self.typesReported = {}
        self.typesToReport = {}
        self.qtNamespaceToReport = None

    def run(self, args):
        self.output = []
        self.currentIName = ""
        self.currentPrintsAddress = True
        self.currentChildType = ""
        self.currentChildNumChild = -1
        self.currentMaxNumChild = -1
        self.currentNumChild = -1
        self.currentValue = None
        self.currentValuePriority = -100
        self.currentValueEncoding = None
        self.currentType = None
        self.currentTypePriority = -100
        self.currentAddress = None
        self.typeformats = {}
        self.formats = {}
        self.useDynamicType = True
        self.expandedINames = {}

        # The guess does not need to be updated during a run()
        # as the result is fixed during that time (ignoring "active"
        # dumpers causing loading of shared objects etc).
        self.currentQtNamespaceGuess = None

        watchers = ""
        resultVarName = ""
        options = []
        varList = []

        self.output.append('data=[')
        for arg in args.split(' '):
            pos = arg.find(":") + 1
            if arg.startswith("options:"):
                options = arg[pos:].split(",")
            elif arg.startswith("vars:"):
                if len(arg[pos:]) > 0:
                    varList = arg[pos:].split(",")
            elif arg.startswith("resultvarname:"):
                resultVarName = arg[pos:]
            elif arg.startswith("expanded:"):
                self.expandedINames = set(arg[pos:].split(","))
            elif arg.startswith("stringcutoff:"):
                self.stringCutOff = int(arg[pos:])
            elif arg.startswith("typeformats:"):
                for f in arg[pos:].split(","):
                    pos = f.find("=")
                    if pos != -1:
                        typeName = self.hexdecode(f[0:pos])
                        self.typeformats[typeName] = int(f[pos+1:])
            elif arg.startswith("formats:"):
                for f in arg[pos:].split(","):
                    pos = f.find("=")
                    if pos != -1:
                        self.formats[f[0:pos]] = int(f[pos+1:])
            elif arg.startswith("watchers:"):
                watchers = self.hexdecode(arg[pos:])

        self.useDynamicType = "dyntype" in options
        self.useFancy = "fancy" in options
        self.forceQtNamespace = "forcens" in options
        self.passExceptions = "pe" in options
        #self.passExceptions = True
        self.autoDerefPointers = "autoderef" in options
        self.partialUpdate = "partial" in options
        self.tooltipOnly = "tooltiponly" in options
        self.fallbackQtVersion = 0x50200
        #warn("NAMESPACE: '%s'" % self.qtNamespace())
        #warn("VARIABLES: %s" % varList)
        #warn("EXPANDED INAMES: %s" % self.expandedINames)
        #warn("WATCHERS: %s" % watchers)
        #warn("PARTIAL: %s" % self.partialUpdate)

        #
        # Locals
        #
        locals = []
        fullUpdateNeeded = True
        if self.partialUpdate and len(varList) == 1 and not self.tooltipOnly:
            #warn("PARTIAL: %s" % varList)
            parts = varList[0].split('.')
            #warn("PARTIAL PARTS: %s" % parts)
            name = parts[1]
            #warn("PARTIAL VAR: %s" % name)
            #fullUpdateNeeded = False
            try:
                frame = gdb.selected_frame()
                item = LocalItem()
                item.iname = "local." + name
                item.name = name
                item.value = frame.read_var(name)
                locals = [item]
                #warn("PARTIAL LOCALS: %s" % locals)
                fullUpdateNeeded = False
            except:
                pass
            varList = []

        if fullUpdateNeeded and not self.tooltipOnly:
            locals = listOfLocals(varList)

        # Take care of the return value of the last function call.
        if len(resultVarName) > 0:
            try:
                item = LocalItem()
                item.name = resultVarName
                item.iname = "return." + resultVarName
                item.value = self.parseAndEvaluate(resultVarName)
                locals.append(item)
            except:
                # Don't bother. It's only supplementary information anyway.
                pass

        for item in locals:
            value = self.downcast(item.value) if self.useDynamicType else item.value
            with OutputSafer(self):
                self.anonNumber = -1

                if item.iname == "local.argv" and str(value.type) == "char **":
                    self.putSpecialArgv(value)
                else:
                    # A "normal" local variable or parameter.
                    with TopLevelItem(self, item.iname):
                        self.put('iname="%s",' % item.iname)
                        self.put('name="%s",' % item.name)
                        self.putItem(value)

        #
        # Watchers
        #
        with OutputSafer(self):
            if len(watchers) > 0:
                for watcher in watchers.split("##"):
                    (exp, iname) = watcher.split("#")
                    self.handleWatch(exp, iname)

        #print('data=[' + locals + sep + watchers + ']\n')

        self.output.append('],typeinfo=[')
        for name in self.typesToReport.keys():
            typeobj = self.typesToReport[name]
            # Happens e.g. for '(anonymous namespace)::InsertDefOperation'
            if not typeobj is None:
                self.output.append('{name="%s",size="%s"}'
                    % (self.hexencode(name), typeobj.sizeof))
        self.output.append(']')
        self.typesToReport = {}

        if "forcens" in options:
            self.qtNamepaceToRport = self.qtNamespace()

        if self.qtNamespaceToReport:
            self.output.append(',qtnamespace="%s"' % self.qtNamespaceToReport)
            self.qtNamespaceToReport = None

        return "".join(self.output)

    def enterSubItem(self, item):
        if not item.iname:
            item.iname = "%s.%s" % (self.currentIName, item.name)
        #warn("INAME %s" % item.iname)
        self.put('{')
        #if not item.name is None:
        if isinstance(item.name, str):
            self.put('name="%s",' % item.name)
        item.savedIName = self.currentIName
        item.savedValue = self.currentValue
        item.savedValuePriority = self.currentValuePriority
        item.savedValueEncoding = self.currentValueEncoding
        item.savedType = self.currentType
        item.savedTypePriority = self.currentTypePriority
        item.savedCurrentAddress = self.currentAddress
        self.currentIName = item.iname
        self.currentValuePriority = -100
        self.currentValueEncoding = None
        self.currentType = ""
        self.currentTypePriority = -100
        self.currentAddress = None

    def exitSubItem(self, item, exType, exValue, exTraceBack):
        #warn(" CURRENT VALUE: %s %s %s" % (self.currentValue,
        #    self.currentValueEncoding, self.currentValuePriority))
        if not exType is None:
            if self.passExceptions:
                showException("SUBITEM", exType, exValue, exTraceBack)
            self.putNumChild(0)
            self.putValue("<not accessible>")
        try:
            #warn("TYPE VALUE: %s" % self.currentValue)
            typeName = stripClassTag(self.currentType)
            #warn("TYPE: '%s'  DEFAULT: '%s' % (typeName, self.currentChildType))

            if len(typeName) > 0 and typeName != self.currentChildType:
                self.put('type="%s",' % typeName) # str(type.unqualified()) ?
            if  self.currentValue is None:
                self.put('value="<not accessible>",numchild="0",')
            else:
                if not self.currentValueEncoding is None:
                    self.put('valueencoded="%d",' % self.currentValueEncoding)
                self.put('value="%s",' % self.currentValue)
        except:
            pass
        if not self.currentAddress is None:
            self.put(self.currentAddress)
        self.put('},')
        self.currentIName = item.savedIName
        self.currentValue = item.savedValue
        self.currentValuePriority = item.savedValuePriority
        self.currentValueEncoding = item.savedValueEncoding
        self.currentType = item.savedType
        self.currentTypePriority = item.savedTypePriority
        self.currentAddress = item.savedCurrentAddress
        return True

    def parseAndEvaluate(self, exp):
        return gdb.parse_and_eval(exp)

    def callHelper(self, value, func, args):
        # args is a tuple.
        arg = ""
        for i in range(len(args)):
            if i:
                arg += ','
            a = args[i]
            if (':' in a) and not ("'" in a):
                arg = "'%s'" % a
            else:
                arg += a

        #warn("CALL: %s -> %s(%s)" % (value, func, arg))
        typeName = stripClassTag(str(value.type))
        if typeName.find(":") >= 0:
            typeName = "'" + typeName + "'"
        # 'class' is needed, see http://sourceware.org/bugzilla/show_bug.cgi?id=11912
        #exp = "((class %s*)%s)->%s(%s)" % (typeName, value.address, func, arg)
        ptr = value.address if value.address else self.pokeValue(value)
        exp = "((%s*)%s)->%s(%s)" % (typeName, ptr, func, arg)
        #warn("CALL: %s" % exp)
        result = gdb.parse_and_eval(exp)
        #warn("  -> %s" % result)
        if not value.address:
            gdb.parse_and_eval("free(0x%x)" % ptr)
        return result

    def childWithName(self, value, name):
        try:
            return value[name]
        except:
            return None

    def makeValue(self, type, init):
        type = "::" + stripClassTag(str(type));
        # Avoid malloc symbol clash with QVector.
        gdb.execute("set $d = (%s*)calloc(sizeof(%s), 1)" % (type, type))
        gdb.execute("set *$d = {%s}" % init)
        value = gdb.parse_and_eval("$d").dereference()
        #warn("  TYPE: %s" % value.type)
        #warn("  ADDR: %s" % value.address)
        #warn("  VALUE: %s" % value)
        return value

    def makeExpression(self, value):
        type = "::" + stripClassTag(str(value.type))
        #warn("  TYPE: %s" % type)
        #exp = "(*(%s*)(&%s))" % (type, value.address)
        exp = "(*(%s*)(%s))" % (type, value.address)
        #warn("  EXP: %s" % exp)
        return exp

    def makeStdString(init):
        # Works only for small allocators, but they are usually empty.
        gdb.execute("set $d=(std::string*)calloc(sizeof(std::string), 2)");
        gdb.execute("call($d->basic_string(\"" + init +
            "\",*(std::allocator<char>*)(1+$d)))")
        value = gdb.parse_and_eval("$d").dereference()
        #warn("  TYPE: %s" % value.type)
        #warn("  ADDR: %s" % value.address)
        #warn("  VALUE: %s" % value)
        return value

    def childAt(self, value, index):
        field = value.type.fields()[index]
        # GDB 7.7 commit b5b08fb4 started to report None as field names.
        if field.name:
            try:
                return value[field.name]
            except:
                return value.cast(field.type)

        # FIXME: Cheat. There seems to be no official way to access
        # the real item, so we pass back the value. That at least
        # enables later ...["name"] style accesses as gdb handles
        # them transparently.
        return value

    def fieldAt(self, type, index):
        return type.fields()[index]

    def simpleValue(self, value):
        return str(value)

    def directBaseClass(self, typeobj, index = 0):
        for f in typeobj.fields():
            if f.is_base_class:
                if index == 0:
                    return f.type
                index -= 1;
        return None

    def directBaseObject(self, value, index = 0):
        for f in value.type.fields():
            if f.is_base_class:
                if index == 0:
                    return value.cast(f.type)
                index -= 1;
        return None

    def checkPointer(self, p, align = 1):
        if not self.isNull(p):
            p.dereference()

    def pointerValue(self, p):
        return toInteger(p)

    def isNull(self, p):
        # The following can cause evaluation to abort with "UnicodeEncodeError"
        # for invalid char *, as their "contents" is being examined
        #s = str(p)
        #return s == "0x0" or s.startswith("0x0 ")
        #try:
        #    # Can fail with: "RuntimeError: Cannot access memory at address 0x5"
        #    return p.cast(self.lookupType("void").pointer()) == 0
        #except:
        #    return False
        try:
            # Can fail with: "RuntimeError: Cannot access memory at address 0x5"
            return toInteger(p) == 0
        except:
            return False

    def templateArgument(self, typeobj, position):
        try:
            # This fails on stock 7.2 with
            # "RuntimeError: No type named myns::QObject.\n"
            return typeobj.template_argument(position)
        except:
            # That's something like "myns::QList<...>"
            return self.lookupType(self.extractTemplateArgument(str(typeobj.strip_typedefs()), position))

    def numericTemplateArgument(self, typeobj, position):
        # Workaround for gdb < 7.1
        try:
            return int(typeobj.template_argument(position))
        except RuntimeError as error:
            # ": No type named 30."
            msg = str(error)
            msg = msg[14:-1]
            # gdb at least until 7.4 produces for std::array<int, 4u>
            # for template_argument(1): RuntimeError: No type named 4u.
            if msg[-1] == 'u':
               msg = msg[0:-1]
            return int(msg)

    def handleWatch(self, exp, iname):
        exp = str(exp)
        escapedExp = self.hexencode(exp);
        #warn("HANDLING WATCH %s, INAME: '%s'" % (exp, iname))
        if exp.startswith("[") and exp.endswith("]"):
            #warn("EVAL: EXP: %s" % exp)
            with TopLevelItem(self, iname):
                self.put('iname="%s",' % iname)
                self.put('wname="%s",' % escapedExp)
                try:
                    list = eval(exp)
                    self.putValue("")
                    self.putNoType()
                    self.putNumChild(len(list))
                    # This is a list of expressions to evaluate
                    with Children(self, len(list)):
                        itemNumber = 0
                        for item in list:
                            self.handleWatch(item, "%s.%d" % (iname, itemNumber))
                            itemNumber += 1
                except RuntimeError as error:
                    warn("EVAL: ERROR CAUGHT %s" % error)
                    self.putValue("<syntax error>")
                    self.putNoType()
                    self.putNumChild(0)
                    self.put("children=[],")
            return

        with TopLevelItem(self, iname):
            self.put('iname="%s",' % iname)
            self.put('wname="%s",' % escapedExp)
            if len(exp) == 0: # The <Edit> case
                self.putValue(" ")
                self.putNoType()
                self.putNumChild(0)
            else:
                try:
                    value = self.parseAndEvaluate(exp)
                    self.putItem(value)
                except RuntimeError:
                    self.currentType = " "
                    self.currentValue = "<no such value>"
                    self.currentChildNumChild = -1
                    self.currentNumChild = 0
                    self.putNumChild(0)

    def intType(self):
        self.cachedIntType = self.lookupType('int')
        self.intType = lambda: self.cachedIntType
        return self.cachedIntType

    def charType(self):
        return self.lookupType('char')

    def sizetType(self):
        return self.lookupType('size_t')

    def charPtrType(self):
        return self.lookupType('char*')

    def voidPtrType(self):
        return self.lookupType('void*')

    def addressOf(self, value):
        return toInteger(value.address)

    def createPointerValue(self, address, pointeeType):
        # This might not always work:
        # a Python 3 based GDB due to the bug addressed in
        # https://sourceware.org/ml/gdb-patches/2013-09/msg00571.html
        try:
            return gdb.Value(address).cast(pointeeType.pointer())
        except:
            # Try _some_ fallback (good enough for the std::complex dumper)
            return gdb.parse_and_eval("(%s*)%s" % (pointeeType, address))

    def intSize(self):
        return 4

    def ptrSize(self):
        self.cachedPtrSize = self.lookupType('void*').sizeof
        self.ptrSize = lambda: self.cachedPtrSize
        return self.cachedPtrSize

    def pokeValue(self, value):
        """
        Allocates inferior memory and copies the contents of value.
        Returns a pointer to the copy.
        """
        # Avoid malloc symbol clash with QVector
        size = value.type.sizeof
        data = value.cast(gdb.lookup_type("unsigned char").array(0, int(size - 1)))
        string = ''.join("\\x%02x" % int(data[i]) for i in range(size))
        exp = '(%s*)memcpy(calloc(%s, 1), "%s", %s)' % (value.type, size, string, size)
        #warn("EXP: %s" % exp)
        return toInteger(gdb.parse_and_eval(exp))


    def createValue(self, address, referencedType):
        try:
            return gdb.Value(address).cast(referencedType.pointer()).dereference()
        except:
            # Try _some_ fallback (good enough for the std::complex dumper)
            return gdb.parse_and_eval("{%s}%s" % (referencedType, address))

    def setValue(self, address, type, value):
        cmd = "set {%s}%s=%s" % (type, address, value)
        gdb.execute(cmd)

    def setValues(self, address, type, values):
        cmd = "set {%s[%s]}%s={%s}" \
            % (type, len(values), address, ','.join(map(str, values)))
        gdb.execute(cmd)

    def selectedInferior(self):
        try:
            # gdb.Inferior is new in gdb 7.2
            self.cachedInferior = gdb.selected_inferior()
        except:
            # Pre gdb 7.4. Right now we don't have more than one inferior anyway.
            self.cachedInferior = gdb.inferiors()[0]

        # Memoize result.
        self.selectedInferior = lambda: self.cachedInferior
        return self.cachedInferior

    def readRawMemory(self, addr, size):
        mem = self.selectedInferior().read_memory(addr, size)
        if sys.version_info[0] >= 3:
            mem.tobytes()
        return mem

    def extractInt64(self, addr):
        return struct.unpack("q", self.readRawMemory(addr, 8))[0]

    def extractInt(self, addr):
        return struct.unpack("i", self.readRawMemory(addr, 4))[0]

    def extractByte(self, addr):
        return struct.unpack("b", self.readRawMemory(addr, 1))[0]

    def findStaticMetaObject(self, typeName):
        return self.findSymbol(typeName + "::staticMetaObject")

    def findSymbol(self, symbolName):
        try:
            result = gdb.lookup_global_symbol(symbolName)
            return result.value() if result else 0
        except:
            pass
        # Older GDB ~7.4
        try:
            address = gdb.parse_and_eval("&'%s'" % symbolName)
            type = gdb.lookup_type(self.qtNamespace() + "QMetaObject")
            return self.createPointerValue(address, type)
        except:
            return 0

    def put(self, value):
        self.output.append(value)

    def childRange(self):
        if self.currentMaxNumChild is None:
            return xrange(0, toInteger(self.currentNumChild))
        return xrange(min(toInteger(self.currentMaxNumChild), toInteger(self.currentNumChild)))

    def isArmArchitecture(self):
        return 'arm' in gdb.TARGET_CONFIG.lower()

    def isQnxTarget(self):
        return 'qnx' in gdb.TARGET_CONFIG.lower()

    def isWindowsTarget(self):
        # We get i686-w64-mingw32
        return 'mingw' in gdb.TARGET_CONFIG.lower()

    def qtVersionString(self):
        try:
            return str(gdb.lookup_symbol("qVersion")[0].value()())
        except:
            pass
        try:
            ns = self.qtNamespace()
            return str(gdb.parse_and_eval("((const char*(*)())'%sqVersion')()" % ns))
        except:
            pass
        return None

    def qtVersion(self):
        try:
            version = self.qtVersionString()
            (major, minor, patch) = version[version.find('"')+1:version.rfind('"')].split('.')
            qtversion = 0x10000 * int(major) + 0x100 * int(minor) + int(patch)
            self.qtVersion = lambda: qtversion
            return qtversion
        except:
            # Use fallback until we have a better answer.
            return self.fallbackQtVersion

    def isQt3Support(self):
        if self.qtVersion() >= 0x050000:
            return False
        else:
            try:
                # This will fail on Qt 4 without Qt 3 support
                gdb.execute("ptype QChar::null", to_string=True)
                self.cachedIsQt3Suport = True
            except:
                self.cachedIsQt3Suport = False

        # Memoize good results.
        self.isQt3Support = lambda: self.cachedIsQt3Suport
        return self.cachedIsQt3Suport

    def putBetterType(self, type):
        self.currentType = str(type)
        self.currentTypePriority = self.currentTypePriority + 1

    def putAddress(self, addr):
        if self.currentPrintsAddress:
            try:
                # addr can be "None", int(None) fails.
                #self.put('addr="0x%x",' % int(addr))
                self.currentAddress = 'addr="0x%x",' % toInteger(addr)
            except:
                pass

    def putNumChild(self, numchild):
        #warn("NUM CHILD: '%s' '%s'" % (numchild, self.currentChildNumChild))
        if numchild != self.currentChildNumChild:
            self.put('numchild="%s",' % numchild)

    def putSimpleValue(self, value, encoding = None, priority = 0):
        self.putValue(value, encoding, priority)

    def putPointerValue(self, value):
        # Use a lower priority
        if value is None:
            self.putEmptyValue(-1)
        else:
            self.putValue("0x%x" % value.cast(
                self.lookupType("unsigned long")), None, -1)

    def putDisplay(self, format, value = None, cmd = None):
        self.put('editformat="%s",' % format)
        if cmd is None:
            if not value is None:
                self.put('editvalue="%s",' % value)
        else:
            self.put('editvalue="%s|%s",' % (cmd, value))

    def isExpandedSubItem(self, component):
        iname = "%s.%s" % (self.currentIName, component)
        #warn("IS EXPANDED: %s in %s" % (iname, self.expandedINames))
        return iname in self.expandedINames

    def stripNamespaceFromType(self, typeName):
        type = stripClassTag(typeName)
        ns = self.qtNamespace()
        if len(ns) > 0 and type.startswith(ns):
            type = type[len(ns):]
        pos = type.find("<")
        # FIXME: make it recognize  foo<A>::bar<B>::iterator?
        while pos != -1:
            pos1 = type.rfind(">", pos)
            type = type[0:pos] + type[pos1+1:]
            pos = type.find("<")
        return type

    def isMovableType(self, type):
        if type.code == PointerCode:
            return True
        if self.isSimpleType(type):
            return True
        return self.isKnownMovableType(self.stripNamespaceFromType(str(type)))

    def putSubItem(self, component, value, tryDynamic=True):
        with SubItem(self, component):
            self.putItem(value, tryDynamic)

    def isSimpleType(self, typeobj):
        code = typeobj.code
        return code == BoolCode \
            or code == CharCode \
            or code == IntCode \
            or code == FloatCode \
            or code == EnumCode

    def simpleEncoding(self, typeobj):
        code = typeobj.code
        if code == BoolCode or code == CharCode:
            return Hex2EncodedInt1
        if code == IntCode:
            if str(typeobj).find("unsigned") >= 0:
                if typeobj.sizeof == 1:
                    return Hex2EncodedUInt1
                if typeobj.sizeof == 2:
                    return Hex2EncodedUInt2
                if typeobj.sizeof == 4:
                    return Hex2EncodedUInt4
                if typeobj.sizeof == 8:
                    return Hex2EncodedUInt8
            else:
                if typeobj.sizeof == 1:
                    return Hex2EncodedInt1
                if typeobj.sizeof == 2:
                    return Hex2EncodedInt2
                if typeobj.sizeof == 4:
                    return Hex2EncodedInt4
                if typeobj.sizeof == 8:
                    return Hex2EncodedInt8
        if code == FloatCode:
            if typeobj.sizeof == 4:
                return Hex2EncodedFloat4
            if typeobj.sizeof == 8:
                return Hex2EncodedFloat8
        return None

    def isReferenceType(self, typeobj):
        return typeobj.code == gdb.TYPE_CODE_REF

    def isStructType(self, typeobj):
        return typeobj.code == gdb.TYPE_CODE_STRUCT

    def putArrayData(self, typeobj, base, n,
            childNumChild = None, maxNumChild = 10000):
        if not self.tryPutArrayContents(typeobj, base, n):
            base = self.createPointerValue(base, typeobj)
            with Children(self, n, typeobj, childNumChild, maxNumChild,
                    base, typeobj.sizeof):
                for i in self.childRange():
                    i = toInteger(i)
                    self.putSubItem(i, (base + i).dereference())

    def isFunctionType(self, type):
        return type.code == MethodCode or type.code == FunctionCode

    def putItem(self, value, tryDynamic=True):
        if value is None:
            # Happens for non-available watchers in gdb versions that
            # need to use gdb.execute instead of gdb.parse_and_eval
            self.putValue("<not available>")
            self.putType("<unknown>")
            self.putNumChild(0)
            return

        type = value.type.unqualified()
        typeName = str(type)
        tryDynamic &= self.useDynamicType
        self.addToCache(type) # Fill type cache
        if tryDynamic:
            self.putAddress(value.address)

        # FIXME: Gui shows references stripped?
        #warn(" ")
        #warn("REAL INAME: %s " % self.currentIName)
        #warn("REAL TYPE: %s " % value.type)
        #warn("REAL CODE: %s " % value.type.code)
        #warn("REAL VALUE: %s " % value)

        if type.code == ReferenceCode:
            try:
                # Try to recognize null references explicitly.
                if toInteger(value.address) == 0:
                    self.putValue("<null reference>")
                    self.putType(typeName)
                    self.putNumChild(0)
                    return
            except:
                pass

            if tryDynamic:
                try:
                    # Dynamic references are not supported by gdb, see
                    # http://sourceware.org/bugzilla/show_bug.cgi?id=14077.
                    # Find the dynamic type manually using referenced_type.
                    value = value.referenced_value()
                    value = value.cast(value.dynamic_type)
                    self.putItem(value)
                    self.putBetterType("%s &" % value.type)
                    return
                except:
                    pass

            try:
                # FIXME: This throws "RuntimeError: Attempt to dereference a
                # generic pointer." with MinGW's gcc 4.5 when it "identifies"
                # a "QWidget &" as "void &" and with optimized out code.
                self.putItem(value.cast(type.target().unqualified()))
                self.putBetterType("%s &" % self.currentType)
                return
            except RuntimeError:
                self.putValue("<optimized out reference>")
                self.putType(typeName)
                self.putNumChild(0)
                return

        if type.code == IntCode or type.code == CharCode:
            self.putType(typeName)
            if value.is_optimized_out:
                self.putValue("<optimized out>")
            elif type.sizeof == 1:
                # Force unadorned value transport for char and Co.
                self.putValue(int(value) & 0xff)
            else:
                self.putValue(value)
            self.putNumChild(0)
            return

        if type.code == FloatCode or type.code == BoolCode:
            self.putType(typeName)
            if value.is_optimized_out:
                self.putValue("<optimized out>")
            else:
                self.putValue(value)
            self.putNumChild(0)
            return

        if type.code == EnumCode:
            self.putType(typeName)
            if value.is_optimized_out:
                self.putValue("<optimized out>")
            else:
                self.putValue("%s (%d)" % (value, value))
            self.putNumChild(0)
            return

        if type.code == ComplexCode:
            self.putType(typeName)
            if value.is_optimized_out:
                self.putValue("<optimized out>")
            else:
                self.putValue("%s" % value)
            self.putNumChild(0)
            return

        if type.code == TypedefCode:
            if typeName in self.qqDumpers:
                self.putType(typeName)
                self.qqDumpers[typeName](self, value)
                return

            type = stripTypedefs(type)
            # The cast can destroy the address?
            #self.putAddress(value.address)
            # Workaround for http://sourceware.org/bugzilla/show_bug.cgi?id=13380
            if type.code == ArrayCode:
                value = self.parseAndEvaluate("{%s}%s" % (type, value.address))
            else:
                try:
                    value = value.cast(type)
                except:
                    self.putValue("<optimized out typedef>")
                    self.putType(typeName)
                    self.putNumChild(0)
                    return

            self.putItem(value)
            self.putBetterType(typeName)
            return

        if type.code == ArrayCode:
            self.putCStyleArray(value)
            return

        if type.code == PointerCode:
            # This could still be stored in a register and
            # potentially dereferencable.
            if value.is_optimized_out:
                self.putValue("<optimized out>")
                return

            self.putFormattedPointer(value)
            return

        if type.code == MethodPointerCode \
                or type.code == MethodCode \
                or type.code == FunctionCode \
                or type.code == MemberPointerCode:
            self.putType(typeName)
            self.putValue(value)
            self.putNumChild(0)
            return

        if typeName.startswith("<anon"):
            # Anonymous union. We need a dummy name to distinguish
            # multiple anonymous unions in the struct.
            self.putType(type)
            self.putValue("{...}")
            self.anonNumber += 1
            with Children(self, 1):
                self.listAnonymous(value, "#%d" % self.anonNumber, type)
            return

        if type.code == StringCode:
            # FORTRAN strings
            size = type.sizeof
            data = self.readMemory(value.address, size)
            self.putValue(data, Hex2EncodedLatin1, 1)
            self.putType(type)

        if type.code != StructCode and type.code != UnionCode:
            warn("WRONG ASSUMPTION HERE: %s " % type.code)
            check(False)


        if tryDynamic:
            self.putItem(self.expensiveDowncast(value), False)
            return

        format = self.currentItemFormat(typeName)
        #warn(" xxxx: %s" % format)

        if (format is None or format >= 1):
            self.putType(typeName)

            nsStrippedType = self.stripNamespaceFromType(typeName)\
                .replace("::", "__")

            # The following block is only needed for D.
            if nsStrippedType.startswith("_A"):
                # DMD v2.058 encodes string[] as _Array_uns long long.
                # With spaces.
                if nsStrippedType.startswith("_Array_"):
                    qdump_Array(self, value)
                    return
                if nsStrippedType.startswith("_AArray_"):
                    qdump_AArray(self, value)
                    return

            #warn(" STRIPPED: %s" % nsStrippedType)
            #warn(" DUMPERS: %s" % self.qqDumpers)
            #warn(" DUMPERS: %s" % (nsStrippedType in self.qqDumpers))
            if self.useFancy:
                dumper = self.qqDumpers.get(nsStrippedType, None)
                if not dumper is None:
                    if tryDynamic:
                        dumper(self, self.expensiveDowncast(value))
                    else:
                        dumper(self, value)
                    return

        # D arrays, gdc compiled.
        if typeName.endswith("[]"):
            n = value["length"]
            base = value["ptr"]
            self.putType(typeName)
            self.putItemCount(n)
            if self.isExpanded():
                self.putArrayData(base.type.target(), base, n)
            return

        #warn("GENERIC STRUCT: %s" % type)
        #warn("INAME: %s " % self.currentIName)
        #warn("INAMES: %s " % self.expandedINames)
        #warn("EXPANDED: %s " % (self.currentIName in self.expandedINames))
        staticMetaObject = self.extractStaticMetaObject(value.type)
        if staticMetaObject:
            self.putQObjectNameValue(value)
        self.putType(typeName)
        self.putEmptyValue()
        self.putNumChild(len(type.fields()))

        if self.currentIName in self.expandedINames:
            innerType = None
            with Children(self, 1, childType=innerType):
                self.putFields(value)
                if staticMetaObject:
                    self.putQObjectGuts(value, staticMetaObject)

    def toBlob(self, value):
        size = toInteger(value.type.sizeof)
        if value.address:
            return self.extractBlob(value.address, size)

        # No address. Possibly the result of an inferior call.
        y = value.cast(gdb.lookup_type("unsigned char").array(0, int(size - 1)))
        buf = bytearray(struct.pack('x' * size))
        for i in range(size):
            buf[i] = int(y[i])

        return Blob(bytes(buf))

    def extractBlob(self, base, size):
        inferior = self.selectedInferior()
        return Blob(inferior.read_memory(base, size))

    def readCString(self, base):
        inferior = self.selectedInferior()
        mem = ""
        while True:
            char = inferior.read_memory(base, 1)[0]
            if not char:
                break
            mem += char
            base += 1
        #if sys.version_info[0] >= 3:
        #    return mem.tobytes()
        return mem

    def putFields(self, value, dumpBase = True):
            fields = value.type.fields()

            #warn("TYPE: %s" % value.type)
            #warn("FIELDS: %s" % fields)
            baseNumber = 0
            for field in fields:
                #warn("FIELD: %s" % field)
                #warn("  BITSIZE: %s" % field.bitsize)
                #warn("  ARTIFICIAL: %s" % field.artificial)

                # Since GDB commit b5b08fb4 anonymous structs get also reported
                # with a 'None' name.
                if field.name is None:
                    if value.type.code == ArrayCode:
                        # An array.
                        type = stripTypedefs(value.type)
                        innerType = type.target()
                        p = value.cast(innerType.pointer())
                        for i in xrange(int(type.sizeof / innerType.sizeof)):
                            with SubItem(self, i):
                                self.putItem(p.dereference())
                            p = p + 1
                    else:
                        # Something without a name.
                        self.anonNumber += 1
                        with SubItem(self, str(self.anonNumber)):
                            self.putItem(value[field])
                    continue

                # Ignore vtable pointers for virtual inheritance.
                if field.name.startswith("_vptr."):
                    with SubItem(self, "[vptr]"):
                        # int (**)(void)
                        n = 100
                        self.putType(" ")
                        self.putValue(value[field.name])
                        self.putNumChild(n)
                        if self.isExpanded():
                            with Children(self):
                                p = value[field.name]
                                for i in xrange(n):
                                    if toInteger(p.dereference()) != 0:
                                        with SubItem(self, i):
                                            self.putItem(p.dereference())
                                            self.putType(" ")
                                            p = p + 1
                    continue

                #warn("FIELD NAME: %s" % field.name)
                #warn("FIELD TYPE: %s" % field.type)
                if field.is_base_class:
                    # Field is base type. We cannot use field.name as part
                    # of the iname as it might contain spaces and other
                    # strange characters.
                    if dumpBase:
                        baseNumber += 1
                        with UnnamedSubItem(self, "@%d" % baseNumber):
                            baseValue = value.cast(field.type)
                            self.put('iname="%s",' % self.currentIName)
                            self.put('name="[%s]",' % field.name)
                            self.putAddress(baseValue.address)
                            self.putItem(baseValue, False)
                elif len(field.name) == 0:
                    # Anonymous union. We need a dummy name to distinguish
                    # multiple anonymous unions in the struct.
                    self.anonNumber += 1
                    self.listAnonymous(value, "#%d" % self.anonNumber,
                        field.type)
                else:
                    # Named field.
                    with SubItem(self, field.name):
                        #bitsize = getattr(field, "bitsize", None)
                        #if not bitsize is None:
                        #    self.put("bitsize=\"%s\"" % bitsize)
                        self.putItem(self.downcast(value[field.name]))


    def listAnonymous(self, value, name, type):
        for field in type.fields():
            #warn("FIELD NAME: %s" % field.name)
            if field.name:
                with SubItem(self, field.name):
                    self.putItem(value[field.name])
            else:
                # Further nested.
                self.anonNumber += 1
                name = "#%d" % self.anonNumber
                #iname = "%s.%s" % (selitem.iname, name)
                #child = SameItem(item.value, iname)
                with SubItem(self, name):
                    self.put('name="%s",' % name)
                    self.putEmptyValue()
                    fieldTypeName = str(field.type)
                    if fieldTypeName.endswith("<anonymous union>"):
                        self.putType("<anonymous union>")
                    elif fieldTypeName.endswith("<anonymous struct>"):
                        self.putType("<anonymous struct>")
                    else:
                        self.putType(fieldTypeName)
                    with Children(self, 1):
                        self.listAnonymous(value, name, field.type)

    def registerDumper(self, funcname, function):
        try:
            #warn("FUNCTION: %s " % funcname)
            #funcname = function.func_name
            if funcname.startswith("qdump__"):
                type = funcname[7:]
                self.qqDumpers[type] = function
                self.qqFormats[type] = self.qqFormats.get(type, "")
            elif funcname.startswith("qform__"):
                type = funcname[7:]
                formats = ""
                try:
                    formats = function()
                except:
                    pass
                self.qqFormats[type] = formats
            elif funcname.startswith("qedit__"):
                type = funcname[7:]
                try:
                    self.qqEditable[type] = function
                except:
                    pass
        except:
            pass

    def bbsetup(self):
        self.qqDumpers = {}
        self.qqFormats = {}
        self.qqEditable = {}
        self.typeCache = {}

        # It's __main__ from gui, gdbbridge from test. Brush over it...
        for modname in ['__main__', 'gdbbridge']:
            dic = sys.modules[modname].__dict__
            for name in dic.keys():
                self.registerDumper(name, dic[name])

        result = "dumpers=["
        for key, value in self.qqFormats.items():
            if key in self.qqEditable:
                result += '{type="%s",formats="%s",editable="true"},' % (key, value)
            else:
                result += '{type="%s",formats="%s"},' % (key, value)
        result += ']'
        return result

    #def threadname(self, maximalStackDepth, objectPrivateType):
    #    e = gdb.selected_frame()
    #    out = ""
    #    ns = self.qtNamespace()
    #    while True:
    #        maximalStackDepth -= 1
    #        if maximalStackDepth < 0:
    #            break
    #        e = e.older()
    #        if e == None or e.name() == None:
    #            break
    #        if e.name() == ns + "QThreadPrivate::start" \
    #                or e.name() == "_ZN14QThreadPrivate5startEPv@4":
    #            try:
    #                thrptr = e.read_var("thr").dereference()
    #                d_ptr = thrptr["d_ptr"]["d"].cast(objectPrivateType).dereference()
    #                try:
    #                    objectName = d_ptr["objectName"]
    #                except: # Qt 5
    #                    p = d_ptr["extraData"]
    #                    if not self.isNull(p):
    #                        objectName = p.dereference()["objectName"]
    #                if not objectName is None:
    #                    data, size, alloc = self.stringData(objectName)
    #                    if size > 0:
    #                         s = self.readMemory(data, 2 * size)
    #
    #                thread = gdb.selected_thread()
    #                inner = '{valueencoded="';
    #                inner += str(Hex4EncodedLittleEndianWithoutQuotes)+'",id="'
    #                inner += str(thread.num) + '",value="'
    #                inner += s
    #                #inner += self.encodeString(objectName)
    #                inner += '"},'
    #
    #                out += inner
    #            except:
    #                pass
    #    return out

    def threadnames(self, maximalStackDepth):
        # FIXME: This needs a proper implementation for MinGW, and only there.
        # Linux, Mac and QNX mirror the objectName() to the underlying threads,
        # so we get the names already as part of the -thread-info output.
        return '[]'
        #out = '['
        #oldthread = gdb.selected_thread()
        #if oldthread:
        #    try:
        #        objectPrivateType = gdb.lookup_type(ns + "QObjectPrivate").pointer()
        #        inferior = self.selectedInferior()
        #        for thread in inferior.threads():
        #            thread.switch()
        #            out += self.threadname(maximalStackDepth, objectPrivateType)
        #    except:
        #        pass
        #    oldthread.switch()
        #return out + ']'


    def importPlainDumper(self, printer):
        name = printer.name.replace("::", "__")
        self.qqDumpers[name] = PlainDumper(printer)
        self.qqFormats[name] = ""

    def importPlainDumpers(self):
        for obj in gdb.objfiles():
            for printers in obj.pretty_printers + gdb.pretty_printers:
                for printer in printers.subprinters:
                    self.importPlainDumper(printer)

    def qtNamespace(self):
        if not self.currentQtNamespaceGuess is None:
            return self.currentQtNamespaceGuess

        # This only works when called from a valid frame.
        try:
            cand = "QArrayData::shared_null"
            symbol = gdb.lookup_symbol(cand)[0]
            if symbol:
                ns = symbol.name[:-len(cand)]
                self.qtNamespaceToReport = ns
                self.qtNamespace = lambda: ns
                return ns
        except:
            pass

        try:
            # This is Qt, but not 5.x.
            cand = "QByteArray::shared_null"
            symbol = gdb.lookup_symbol(cand)[0]
            if symbol:
                ns = symbol.name[:-len(cand)]
                self.qtNamespaceToReport = ns
                self.qtNamespace = lambda: ns
                self.fallbackQtVersion = 0x40800
                return ns
        except:
            pass

        self.currentQtNamespaceGuess = ""
        return ""

    def bbedit(self, args):
        (typeName, expr, data) = args.split(',')
        d = Dumper()
        typeName = d.hexdecode(typeName)
        ns = self.qtNamespace()
        if typeName.startswith(ns):
            typeName = typeName[len(ns):]
        typeName = typeName.replace("::", "__")
        pos = typeName.find('<')
        if pos != -1:
            typeName = typeName[0:pos]
        expr = d.hexdecode(expr)
        data = d.hexdecode(data)
        if typeName in self.qqEditable:
            #self.qqEditable[typeName](self, expr, data)
            value = gdb.parse_and_eval(expr)
            self.qqEditable[typeName](self, value, data)
        else:
            cmd = "set variable (%s)=%s" % (expr, data)
            gdb.execute(cmd)

    def hasVTable(self, type):
        fields = type.fields()
        if len(fields) == 0:
            return False
        if fields[0].is_base_class:
            return hasVTable(fields[0].type)
        return str(fields[0].type) ==  "int (**)(void)"

    def dynamicTypeName(self, value):
        if self.hasVTable(value.type):
            #vtbl = str(gdb.parse_and_eval("{int(*)(int)}%s" % int(value.address)))
            try:
                # Fails on 7.1 due to the missing to_string.
                vtbl = gdb.execute("info symbol {int*}%s" % int(value.address),
                    to_string = True)
                pos1 = vtbl.find("vtable ")
                if pos1 != -1:
                    pos1 += 11
                    pos2 = vtbl.find(" +", pos1)
                    if pos2 != -1:
                        return vtbl[pos1 : pos2]
            except:
                pass
        return str(value.type)

    def downcast(self, value):
        try:
            return value.cast(value.dynamic_type)
        except:
            pass
        #try:
        #    return value.cast(self.lookupType(self.dynamicTypeName(value)))
        #except:
        #    pass
        return value

    def expensiveDowncast(self, value):
        try:
            return value.cast(value.dynamic_type)
        except:
            pass
        try:
            return value.cast(self.lookupType(self.dynamicTypeName(value)))
        except:
            pass
        return value

    def addToCache(self, type):
        typename = str(type)
        if typename in self.typesReported:
            return
        self.typesReported[typename] = True
        self.typesToReport[typename] = type

    def enumExpression(self, enumType, enumValue):
        return self.qtNamespace() + "Qt::" + enumValue

    def lookupType(self, typestring):
        type = self.typeCache.get(typestring)
        #warn("LOOKUP 1: %s -> %s" % (typestring, type))
        if not type is None:
            return type

        if typestring == "void":
            type = gdb.lookup_type(typestring)
            self.typeCache[typestring] = type
            self.typesToReport[typestring] = type
            return type

        #try:
        #    type = gdb.parse_and_eval("{%s}&main" % typestring).type
        #    if not type is None:
        #        self.typeCache[typestring] = type
        #        self.typesToReport[typestring] = type
        #        return type
        #except:
        #    pass

        # See http://sourceware.org/bugzilla/show_bug.cgi?id=13269
        # gcc produces "{anonymous}", gdb "(anonymous namespace)"
        # "<unnamed>" has been seen too. The only thing gdb
        # understands when reading things back is "(anonymous namespace)"
        if typestring.find("{anonymous}") != -1:
            ts = typestring
            ts = ts.replace("{anonymous}", "(anonymous namespace)")
            type = self.lookupType(ts)
            if not type is None:
                self.typeCache[typestring] = type
                self.typesToReport[typestring] = type
                return type

        #warn(" RESULT FOR 7.2: '%s': %s" % (typestring, type))

        # This part should only trigger for
        # gdb 7.1 for types with namespace separators.
        # And anonymous namespaces.

        ts = typestring
        while True:
            #warn("TS: '%s'" % ts)
            if ts.startswith("class "):
                ts = ts[6:]
            elif ts.startswith("struct "):
                ts = ts[7:]
            elif ts.startswith("const "):
                ts = ts[6:]
            elif ts.startswith("volatile "):
                ts = ts[9:]
            elif ts.startswith("enum "):
                ts = ts[5:]
            elif ts.endswith(" const"):
                ts = ts[:-6]
            elif ts.endswith(" volatile"):
                ts = ts[:-9]
            elif ts.endswith("*const"):
                ts = ts[:-5]
            elif ts.endswith("*volatile"):
                ts = ts[:-8]
            else:
                break

        if ts.endswith('*'):
            type = self.lookupType(ts[0:-1])
            if not type is None:
                type = type.pointer()
                self.typeCache[typestring] = type
                self.typesToReport[typestring] = type
                return type

        try:
            #warn("LOOKING UP '%s'" % ts)
            type = gdb.lookup_type(ts)
        except RuntimeError as error:
            #warn("LOOKING UP '%s': %s" % (ts, error))
            # See http://sourceware.org/bugzilla/show_bug.cgi?id=11912
            exp = "(class '%s'*)0" % ts
            try:
                type = self.parseAndEvaluate(exp).type.target()
            except:
                # Can throw "RuntimeError: No type named class Foo."
                pass
        except:
            #warn("LOOKING UP '%s' FAILED" % ts)
            pass

        if not type is None:
            self.typeCache[typestring] = type
            self.typesToReport[typestring] = type
            return type

        # This could still be None as gdb.lookup_type("char[3]") generates
        # "RuntimeError: No type named char[3]"
        self.typeCache[typestring] = type
        self.typesToReport[typestring] = type
        return type



# Global instance.
theDumper = Dumper()

#######################################################################
#
# Internal profiling
#
#######################################################################

def p1(args):
    import tempfile
    import cProfile
    tempDir = tempfile.gettempdir() + "/bbprof"
    cProfile.run('bb("%s")' % args, tempDir)
    import pstats
    pstats.Stats(tempDir).sort_stats('time').print_stats()
    return ""

registerCommand("p1", p1)

def p2(args):
    import timeit
    return timeit.repeat('bb("%s")' % args,
        'from __main__ import bb', number=10)

registerCommand("p2", p2)

def profileit(args):
    eval(args)

def profile(args):
    import timeit
    return timeit.repeat('profileit("%s")' % args, 'from __main__ import profileit', number=10000)

registerCommand("pp", profile)

#######################################################################
#
# ThreadNames Command
#
#######################################################################

def threadnames(arg):
    return theDumper.threadnames(int(arg))

registerCommand("threadnames", threadnames)

#######################################################################
#
# Mixed C++/Qml debugging
#
#######################################################################

def qmlb(args):
    # executeCommand(command, to_string=True).split("\n")
    warn("RUNNING: break -f QScript::FunctionWrapper::proxyCall")
    output =  gdb.execute("rbreak -f QScript::FunctionWrapper::proxyCall", to_string=True).split("\n")
    warn("OUTPUT: %s " % output)
    bp = output[0]
    warn("BP: %s " % bp)
    # BP: ['Breakpoint 3 at 0xf166e7: file .../qscriptfunction.cpp, line 75.\\n'] \n"
    pos = bp.find(' ') + 1
    warn("POS: %s " % pos)
    nr = bp[bp.find(' ') + 1 : bp.find(' at ')]
    warn("NR: %s " % nr)
    return bp

registerCommand("qmlb", qmlb)

bbsetup()
