
// include LLVM Header files
#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/Optional.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"


#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include <tuple>
#include <cmath>
#include <cstdio>
#include <memory>

using namespace std;
using namespace llvm;

bool OPT = false;  

#include "utils.hpp"

class lType;
class astNode;
class symbolTable;


#include "symbolTable.hpp"
#include "types/lType_base.hpp"
#include "ast/astNode_base.hpp"

#include "types/lType.hpp"
#include "ast/astNode.hpp"
#include "codegen/module.hpp"


#include "parser/token.hpp"
#include "parser/lexer.hpp"
#include "parser/parser.hpp"

#include "preprocess.hpp"
#include "getMod.hpp"


// builtin functions 
// TODO: move to seperate file
lType* print(vector<lType*> inp){
  inp[0]->print();
  cout << endl;
  return new lNone;
}

lType* input(vector<lType*> inp){
  string in;
  cin >> in;
  return new stringType(in);
}


int main(int argc ,char** argv) {
  bool INFO = 0;
  bool INTERP = 1;
  bool IR = 0;
  bool LINK = 0;

  
  string FILE;
  string INP_FILE = "main.w";
  
  if (argc == 1){
    cout << "WOAJ: A custom built programming language" <<endl;
    cout << "Usage" <<endl;
    cout << "./main [output filename]" <<endl;
    cout << " -i     Run in debug/info mode" <<endl;
    cout << " -c     enable compiler mode" <<endl;
    cout << " --ir   print LLVM IR" <<endl;
    cout << " -l     link object file" <<endl;
    cout << " -O     optimize" <<endl;
    cout << "Note: main.w is currently hardcoded input file" << endl;
    
    return 0;
  }
  
  for (int i = 1; i < argc; i++){
    string arg = argv[i];
    if ( arg == "-i" || arg == "--info"){
      INFO = true;
    }
    else if (arg == "-c"){
      INTERP = 0;
    }
    else if (arg == "--ir"){
      IR = true;
    }
    else if (arg == "-l"){
      LINK = true;
    }
    else if (arg == "-O"){
      OPT = true;
    }
    else{
      FILE = arg;
    }
  }
  
  if(FILE == "" && LINK) { FILE = "a.out";}
  else if (FILE == "") { FILE = "out.o"; }
  if(!INTERP){
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
  }
  vector<string> b;
  map<string,lType*> var_map;  //initialize map for symbol table
  symbolTable gst(var_map); //global symbol table
  if (INTERP){
    gst.set("print", new builtInFn(1,&print));
    gst.set("input", new builtInFn(1,&input));
  }
  initModule();
  
  string inp = readFile(INP_FILE);
  
  
  auto tp = find_imports(inp);
  inp = get<0>(tp);
  b = get<1>(tp);
  for (string mod_name: b){
    if (INTERP) { getMod(mod_name,gst); }
  };
  lexer lex (inp);
  vector<token> tokens = lex.gen_toks();
  if (INFO){  
    for(auto info: tokens){ info.print(); }
    cout<<endl;
  }
  parser main_parser (tokens);
  while(1){
    astNode* exp = main_parser.logical_expr();
    if (INFO){ exp->print(); cout << endl; }
    if (!INTERP) { exp->codegen(); }
    if (INTERP) { exp->exec(gst); }
    if(main_parser.cur.type == tt("NONE")){break; }
  }
  if (!INTERP){
    InitializeAllTargetInfos();
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmParsers();
    InitializeAllAsmPrinters();


    if (IR) { mod->print(errs(),nullptr); }
    if (!LINK) { genObj(FILE); }
    if (LINK) {
      char tmp_file[] = "/tmp/fileXXXXXX";
      int _file_desc = mkstemp(tmp_file);
      genObj(ff);
      system(("clang++ libio.so "+ string(tmp_file)+" -o " + FILE +" -Wl,-rpath,`pwd`").c_str());
    }
  }
  
}
