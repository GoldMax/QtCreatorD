############################################################################
#
# Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
# Contact: http://www.qt-project.org/legal
#
# This file is part of Qt Creator.
#
# Commercial License Usage
# Licensees holding valid commercial Qt licenses may use this file in
# accordance with the commercial license agreement provided with the
# Software or, alternatively, in accordance with the terms contained in
# a written agreement between you and Digia.  For licensing terms and
# conditions see http://qt.digia.com/licensing.  For further information
# use the contact form at http://qt.digia.com/contact-us.
#
# GNU Lesser General Public License Usage
# Alternatively, this file may be used under the terms of the GNU Lesser
# General Public License version 2.1 as published by the Free Software
# Foundation and appearing in the file LICENSE.LGPL included in the
# packaging of this file.  Please review the following information to
# ensure the GNU Lesser General Public License version 2.1 requirements
# will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
#
# In addition, as a special exception, Digia gives you certain additional
# rights.  These rights are described in the Digia Qt LGPL Exception
# version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
#
#############################################################################

from dumper import *

#######################################################################
#
# SSE
#
#######################################################################

def qform____m128():
    return "As Floats,As Doubles"

def qdump____m128(d, value):
    d.putEmptyValue()
    d.putNumChild(1)
    if d.isExpanded():
        format = d.currentItemFormat()
        if format == 2: # As Double
            d.putArrayData(d.lookupType("double"), value.address, 2)
        else: # Default, As float
            d.putArrayData(d.lookupType("float"), value.address, 4)


#######################################################################
#
# Eigen
#
#######################################################################

#def qform__Eigen__Matrix():
#    return "Transposed"

def qdump__Eigen__Matrix(d, value):
    innerType = d.templateArgument(value.type, 0)
    storage = value["m_storage"]
    options = d.numericTemplateArgument(value.type, 3)
    rowMajor = (int(options) & 0x1)
    argRow = d.numericTemplateArgument(value.type, 1)
    argCol = d.numericTemplateArgument(value.type, 2)
    nrows = value["m_storage"]["m_rows"] if argRow == -1 else int(argRow)
    ncols = value["m_storage"]["m_cols"] if argCol == -1 else int(argCol)
    p = storage["m_data"]
    if d.isStructType(p.type): # Static
        p = p["array"].cast(innerType.pointer())
    d.putValue("(%s x %s), %s" % (nrows, ncols, ["ColumnMajor", "RowMajor"][rowMajor]))
    d.putField("keeporder", "1")
    d.putNumChild(nrows * ncols)

    limit = 10000
    nncols = min(ncols, limit)
    nnrows = min(nrows, limit * limit / nncols)
    if d.isExpanded():
        #format = d.currentItemFormat() # format == 1 is "Transposed"
        with Children(d, nrows * ncols, childType=innerType):
            if ncols == 1 or nrows == 1:
                for i in range(0, min(nrows * ncols, 10000)):
                    d.putSubItem(i, (p + i).dereference())
            elif rowMajor == 1:
                s = 0
                for i in range(0, nnrows):
                    for j in range(0, nncols):
                        v = (p + i * ncols + j).dereference()
                        d.putNamedSubItem(s, v, "[%d,%d]" % (i, j))
                        s = s + 1
            else:
                s = 0
                for j in range(0, nncols):
                    for i in range(0, nnrows):
                        v = (p + i + j * nrows).dereference()
                        d.putNamedSubItem(s, v, "[%d,%d]" % (i, j))
                        s = s + 1


#######################################################################
#
# D
#
#######################################################################

def encodeDArray(d, p, innerType, suffix, length):
    t = d.lookupType(innerType)
    p = p.cast(t.pointer())
    #limit = self.findFirstZero(p, self.stringCutOff)
    s = d.readMemory(p, length * t.sizeof)
    #if limit > self.stringCutOff:
    #    s += suffix
    return s

def encodeDCharArray(d, p, length):
    return encodeDArray(d, p, "unsigned char", "2e2e2e", length)

def encodeDChar2Array(d, p, length):
    return encodeDArray(d, p, "unsigned short", "2e002e002e00", length)

def encodeDChar4Array(d, p, length):
    return encodeDArray(d, p, "unsigned int", "2e0000002e0000002e000000", length)


def cleanDType(type):
    return stripClassTag(str(type)).replace("uns long long", "string")

def qdump__Array_char(d, value):
    n = value["length"]
    p = value["ptr"]
    d.putType("string")
    if n==0:
        if d.isNull(p):
            d.putValue("null")
        else:
            d.putValue("<empty>")
    else:
        d.putValue(encodeDCharArray(d,p,n), Hex2EncodedUtf8)
    d.putNumChild(2)
    if d.isExpanded():
      with Children(d):
        with SubItem(d, "length"):
            d.putItem(n)
        with SubItem(d, "ptr"):
            d.putType("char*")
             if d.isNull(p):
                d.putValue("0x0");
            else:
                d.putValue("@%s" % p.address)
            d.putNumChild(n)
            if d.isExpanded():
                innerType = p.dereference().type
                with Children(d, n, childType="char"):
                    for i in d.childRange():
                      with SubItem(d,i):
                        d.putType(innerType)
                        d.putValue((p+i).dereference())
                        d.putNumChild(0)

def qdump__Array_dchar(d, value):
    n = value["length"]
    p = value["ptr"]
    d.putType("dstring")
    if n==0:
        if d.isNull(p):
            d.putValue("null")
        else:
            d.putValue("<empty>")
    else:
        d.putValue(encodeDChar4Array(d,p,n), Hex8EncodedLittleEndian)
    d.putNumChild(2)
    if d.isExpanded():
      with Children(d):
        with SubItem(d, "length"):
            d.putItem(n)
        with SubItem(d, "ptr"):
            d.putType("dchar*")
            if d.isNull(p):
                d.putValue("0x0");
            else:
                d.putValue("@%s" % p.address)
            d.putNumChild(n)
            if d.isExpanded():
                innerType = p.dereference().type
                with Children(d, n, childType="dchar"):
                    for i in d.childRange():
                      with SubItem(d,i):
                        d.putType(innerType)
                        d.putValue((p+i).dereference())
                        d.putNumChild(0)

def qdump__Array_wchar_t(d, value):
    n = value["length"]
    p = value["ptr"]
    d.putType("wstring")
    if n==0:
        if d.isNull(p):
            d.putValue("null")
        else:
            d.putValue("<empty>")
    else:
        d.putValue(encodeDChar2Array(d,p,n), Hex4EncodedLittleEndian)
    d.putNumChild(2)
    if d.isExpanded():
      with Children(d):
        with SubItem(d, "length"):
            d.putItem(n)
        with SubItem(d, "ptr"):
            d.putType("wchar*")
            if d.isNull(p):
                d.putValue("0x0");
            else:
                d.putValue("@%s" % p.address)
            d.putNumChild(n)
            if d.isExpanded():
                innerType = p.dereference().type
                with Children(d, n, childType="wchar"):
                    for i in d.childRange():
                      with SubItem(d,i):
                        d.putType(innerType)
                        d.putValue((p+i).dereference())
                        d.putNumChild(0)

    #d.putType("XXX_" + cleanDType(value.type)[7:])
    #for xx in value:
    #warn("Warn type:%s " % p.type)
    #warn("Warn p.dereference:%s " % p.dereference())
    #warn("Warn p.dereferenceValue:%s " % p.dereferenceValue())
    #warn("Warn newValue:%s " % d.createValue(p.address + 1, "char"))
    #warn("Warn p.address:%s " % p.address)
    #warn("Warn value.address:%s " % value.address)
    #warn("Warn d.dereferenceValue(p):%s " % d.dereferenceValue(p))
    #warn("Warn d.dereferenceValue(p.address):%s " % d.dereferenceValue(p.address))
    #warn("Warn cleanAddress(p.address):%s " % cleanAddress(p.address))

def qdump_Array(d, value):
    n = value["length"]
    p = value["ptr"]
    #t = cleanDType(value.type)[7:]
    t = stripClassTag(str(value.type))[7:]
    warn("Inner Type: %s" % t)
    if t == "char":
        qdump__Array_char(d, value)
    elif t == "wchar_t":
        qdump__Array_wchar_t(d, value)
    elif t == "dchar":
        qdump__Array_dchar(d, value)
    else:
        innerType = p.dereference().dereference().type
        d.putType("%s[%d]" % (innerType, n))
        warn("FirstValue %s" % p.dereference())    
        d.putEmptyValue()
        d.putNumChild(n)
        #innerType = p.type
        if d.isExpanded():
            with Children(d, n, childType=innerType):
                for i in range(0, n):
                    d.putSubItem(i, (p+1).dereference())

def qdump_AArray(d, value):
    #n = value["length"]
    # This ends up as _AArray_<key>_<value> with a single .ptr
    # member of type void *. Not much that can be done here.
    p = value["ptr"]
    t = cleanDType(value.type)[8:]
    d.putType("%s]" % t.replace("_", "["))
    d.putEmptyValue()
    d.putNumChild(1)
    if d.isExpanded():
        with Children(d, 1):
                d.putSubItem("ptr", p)

#######################################################################
#
# Display Test
#
#######################################################################

if False:

    # FIXME: Make that work
    def qdump__Color(d, value):
        v = value
        d.putValue("(%s, %s, %s; %s)" % (v["r"], v["g"], v["b"], v["a"]))
        d.putPlainChildren(value)

    def qdump__Color_(d, value):
        v = value
        d.putValue("(%s, %s, %s; %s)" % (v["r"], v["g"], v["b"], v["a"]))
        if d.isExpanded():
            with Children(d):
                with SubItem(d, "0"):
                    d.putItem(v["r"])
                with SubItem(d, "1"):
                    d.putItem(v["g"])
                with SubItem(d, "2"):
                    d.putItem(v["b"])
                with SubItem(d, "3"):
                    d.putItem(v["a"])


if False:

    def qform__basic__Function():
        return "Normal,Displayed"

    def qdump__basic__Function(d, value):
        min = value["min"]
        max = value["max"]
        data, size, alloc = d.byteArrayData(value["var"])
        var = extractCString(data, 0)
        data, size, alloc = d.byteArrayData(value["f"])
        f = extractCString(data, 0)
        d.putValue("%s, %s=%f..%f" % (f, var, min, max))
        d.putNumChild(0)
        format = d.currentItemFormat()
        if format == 1:
            d.putDisplay(StopDisplay)
        elif format == 2:
            input = "plot [%s=%f:%f] %s" % (var, min, max, f)
            d.putDisplay(DisplayProcess, input, "gnuplot")


if False:

    def qdump__tree_entry(d, value):
        d.putValue("len: %s, offset: %s, type: %s" %
            (value["blocklength"], value["offset"], value["type"]))
        d.putNumChild(0)

    def qdump__tree(d, value):
        count = value["count"]
        entries = value["entries"]
        base = value["base"].cast(d.charPtrType())
        d.putItemCount(count)
        d.putNumChild(count)
        if d.isExpanded():
          with Children(d):
            with SubItem(d, "tree"):
              d.putEmptyValue()
              d.putNoType()
              d.putNumChild(1)
              if d.isExpanded():
                with Children(d):
                  for i in xrange(count):
                      d.putSubItem(Item(entries[i], iname))
            with SubItem(d, "data"):
              d.putEmptyValue()
              d.putNoType()
              d.putNumChild(1)
              if d.isExpanded():
                 with Children(d):
                    for i in xrange(count):
                      with SubItem(d, i):
                        entry = entries[i]
                        mpitype = str(entry["type"])
                        d.putType(mpitype)
                        length = int(entry["blocklength"])
                        offset = int(entry["offset"])
                        d.putValue("%s items at %s" % (length, offset))
                        if mpitype == "MPI_INT":
                          innerType = "int"
                        elif mpitype == "MPI_CHAR":
                          innerType = "char"
                        elif mpitype == "MPI_DOUBLE":
                          innerType = "double"
                        else:
                          length = 0
                        d.putNumChild(length)
                        if d.isExpanded():
                           with Children(d):
                              t = d.lookupType(innerType).pointer()
                              p = (base + offset).cast(t)
                              for j in range(length):
                                d.putSubItem(j, p.dereference())

    #struct KRBase
    #{
    #    enum Type { TYPE_A, TYPE_B } type;
    #    KRBase(Type _type) : type(_type) {}
    #};
    #
    #struct KRA : KRBase { int x; int y; KRA():KRBase(TYPE_A),x(1),y(32) {} };
    #struct KRB : KRBase { KRB():KRBase(TYPE_B) {}  };
    #
    #void testKR()
    #{
    #    KRBase *ptr1 = new KRA;
    #    KRBase *ptr2 = new KRB;
    #    ptr2 = new KRB;
    #}

    def qdump__KRBase(d, value):
        if getattr(value, "__nested__", None) is None:
            base = ["KRA", "KRB"][int(value["type"])]
            nest = value.cast(d.lookupType(base))
            nest.__nested__ = True
            warn("NEST %s " % dir(nest))
            d.putItem(nest)
        else:
            d.putName("type")
            d.putValue(value["type"])
            d.putNoType()



if False:
    def qdump__bug5106__A5106(d, value):
        d.putName("a")
        d.putValue("This is the value: %s" % value["m_a"])
        d.putNoType()
        d.putNumChild(0)


if False:
    def qdump__bug6933__Base(d, value):
        d.putValue("foo")
        d.putPlainChildren(value)

if False:
    def qdump__gdb13393__Base(d, value):
        d.putValue("Base (%s)" % value["a"])
        d.putType(value.type)
        d.putPlainChildren(value)

    def qdump__gdb13393__Derived(d, value):
        d.putValue("Derived (%s, %s)" % (value["a"], value["b"]))
        d.putType(value.type)
        d.putPlainChildren(value)


def qdump__KDSoapValue1(d, value):
    inner = value["d"]["d"].dereference()
    d.putStringValue(inner["m_name"])
    if d.isExpanded():
        with Children(d):
            d.putFields(inner)

def qdump__KDSoapValue(d, value):
    p = (value.cast(lookupType("char*")) + 4).dereference().cast(lookupType("QString"))
    d.putStringValue(p)
    if d.isExpanded():
        with Children(d):
            data = value["d"]["d"].dereference()
            d.putFields(data)
