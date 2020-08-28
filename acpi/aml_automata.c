#include <aml_automata.h>
#include <types.h>
/*
    为了实现ACPI自动机，我们应该实现以下单词的识别：
    LeadNameChar := ‘A’-‘Z’ | ‘_’
    DigitChar := ‘0’-‘9’
    NameChar := DigitChar | LeadNameChar
    RootChar := ‘\’
    ParentPrefixChar := ‘^’
    ‘A’-‘Z’ := 0x41 - 0x5A
    ‘_’ := 0x5F
    ‘0’-‘9’ := 0x30 - 0x39
    ‘\’ := 0x5C
    ‘^’ := 0x5E
    NameSeg := <LeadNameChar NameChar NameChar NameChar>
    // Notice that NameSegs shorter than 4 characters are filled with
    // trailing underscores (‘_’s).
    NameString := <RootChar NamePath> | <PrefixPath NamePath>
    PrefixPath := Nothing | <‘^’ PrefixPath>
    NamePath := NameSeg | DualNamePath | MultiNamePath | NullName
    DualNamePath := DualNamePrefix NameSeg NameSeg
    DualNamePrefix := 0x2E
    MultiNamePath := MultiNamePrefix SegCount NameSeg(SegCount)
    MultiNamePrefix := 0x2F
    SegCount := ByteData
    ComputationalData := ByteConst | WordConst | DWordConst | QWordConst | String | 
    ConstObj | RevisionOp | DefBuffer
    DataObject := ComputationalData | DefPackage | DefVarPackage
    DataRefObject := DataObject | ObjectReference | DDBHandle
    ByteConst := BytePrefix ByteData
    BytePrefix := 0x0A
    WordConst := WordPrefix WordData
    WordPrefix := 0x0B
*/



//返回值：成功，返回总共占用的tree_node个数（不是字节数！）;失败，返回-1.
U64 aml_pass_1(U8* bytecode,tree* buffer)
{
    return 0;
}