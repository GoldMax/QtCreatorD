from dumper import *

#######################################################################
#
# D
#
#######################################################################

def echo(message):
    print("XXX: %s\n" % message.encode("latin1"))

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

def qdump___Array_char(d, value):
    echo("ZZZ")
    d.putValue("XXX")
    d.putType("string")
    """n = value["length"]
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
"""
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
