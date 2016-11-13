##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=modVer
ConfigurationName      :=Debug
WorkspacePath          :=C:/work/projects/modVer
ProjectPath            :=C:/work/projects/modVer
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=uri
Date                   :=13/11/2016
CodeLitePath           :="C:/Program Files/CodeLite"
LinkerName             :="C:/Program Files/LLVM/bin/clang++.exe"
SharedObjectLinkerName :="C:/Program Files/LLVM/bin/clang++.exe" -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="modVer.txt"
PCHCompileFlags        :=
MakeDirCommand         :=makedir
RcCmpOptions           := 
RcCompilerName         :=C:/MinGW/bin/windres.exe
LinkOptions            :=  -O0
IncludePath            := $(IncludeSwitch)c:\MinGW\lib\gcc\mingw32\5.3.0\include\c++ $(IncludeSwitch)c:\MinGW\lib\gcc\mingw32\5.3.0\include\c++\mingw32 $(IncludeSwitch)c:\MinGW\lib\gcc\mingw32\5.3.0\include\c++\backward $(IncludeSwitch)c:\MinGW\lib\gcc\mingw32\5.3.0\include $(IncludeSwitch)c:\MinGW\include $(IncludeSwitch)c:\MinGW\lib\gcc\mingw32\5.3.0\include-fixed $(IncludeSwitch)c:\MinGW\mingw32\include  $(IncludeSwitch). $(IncludeSwitch)include $(IncludeSwitch). $(IncludeSwitch)src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). $(LibraryPathSwitch). $(LibraryPathSwitch)Debug 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := "C:/Program Files/LLVM/bin/llvm-ar.exe" rcu
CXX      := "C:/Program Files/LLVM/bin/clang++.exe"
CC       := "C:/Program Files/LLVM/bin/clang.exe"
CXXFLAGS :=  -g -std=c++14 -Wall -pedantic -Wextra -W $(Preprocessors)
CFLAGS   :=   $(Preprocessors)
ASFLAGS  := 
AS       := "C:/Program Files/LLVM/bin/as.exe"


##
## User defined environment variables
##
CodeLiteDir:=C:\Program Files\CodeLite
Objects0=$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_common_Log.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@$(MakeDirCommand) "./Debug"


$(IntermediateDirectory)/.d:
	@$(MakeDirCommand) "./Debug"

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/main.cpp$(ObjectSuffix): main.cpp $(IntermediateDirectory)/main.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/main.cpp$(DependSuffix): main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/main.cpp$(DependSuffix) -MM main.cpp

$(IntermediateDirectory)/main.cpp$(PreprocessSuffix): main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/main.cpp$(PreprocessSuffix)main.cpp

$(IntermediateDirectory)/src_common_Log.cpp$(ObjectSuffix): src/common/Log.cpp $(IntermediateDirectory)/src_common_Log.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/common/Log.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_common_Log.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_common_Log.cpp$(DependSuffix): src/common/Log.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_common_Log.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_common_Log.cpp$(DependSuffix) -MM src/common/Log.cpp

$(IntermediateDirectory)/src_common_Log.cpp$(PreprocessSuffix): src/common/Log.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_common_Log.cpp$(PreprocessSuffix)src/common/Log.cpp

$(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(ObjectSuffix): src/frontend/parser/ParserBase.cpp $(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/frontend/parser/ParserBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(DependSuffix): src/frontend/parser/ParserBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(DependSuffix) -MM src/frontend/parser/ParserBase.cpp

$(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(PreprocessSuffix): src/frontend/parser/ParserBase.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_frontend_parser_ParserBase.cpp$(PreprocessSuffix)src/frontend/parser/ParserBase.cpp

$(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(ObjectSuffix): src/frontend/parser/TextPos.cpp $(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/frontend/parser/TextPos.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(DependSuffix): src/frontend/parser/TextPos.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(DependSuffix) -MM src/frontend/parser/TextPos.cpp

$(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(PreprocessSuffix): src/frontend/parser/TextPos.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_frontend_parser_TextPos.cpp$(PreprocessSuffix)src/frontend/parser/TextPos.cpp

$(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(ObjectSuffix): src/frontend/parser/Tokenizer.cpp $(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/frontend/parser/Tokenizer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(DependSuffix): src/frontend/parser/Tokenizer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(DependSuffix) -MM src/frontend/parser/Tokenizer.cpp

$(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(PreprocessSuffix): src/frontend/parser/Tokenizer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_frontend_parser_Tokenizer.cpp$(PreprocessSuffix)src/frontend/parser/Tokenizer.cpp

$(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(ObjectSuffix): src/frontend/boogie/coco/Parser.cpp $(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/frontend/boogie/coco/Parser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(DependSuffix): src/frontend/boogie/coco/Parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(DependSuffix) -MM src/frontend/boogie/coco/Parser.cpp

$(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(PreprocessSuffix): src/frontend/boogie/coco/Parser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_frontend_boogie_coco_Parser.cpp$(PreprocessSuffix)src/frontend/boogie/coco/Parser.cpp

$(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(ObjectSuffix): src/frontend/boogie/coco/Scanner.cpp $(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/frontend/boogie/coco/Scanner.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(DependSuffix): src/frontend/boogie/coco/Scanner.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(DependSuffix) -MM src/frontend/boogie/coco/Scanner.cpp

$(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(PreprocessSuffix): src/frontend/boogie/coco/Scanner.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_frontend_boogie_coco_Scanner.cpp$(PreprocessSuffix)src/frontend/boogie/coco/Scanner.cpp

$(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(ObjectSuffix): src/frontend/boogie/manual/Parser.cpp $(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "C:/work/projects/modVer/src/frontend/boogie/manual/Parser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(DependSuffix): src/frontend/boogie/manual/Parser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(DependSuffix) -MM src/frontend/boogie/manual/Parser.cpp

$(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(PreprocessSuffix): src/frontend/boogie/manual/Parser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_frontend_boogie_manual_Parser.cpp$(PreprocessSuffix)src/frontend/boogie/manual/Parser.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


