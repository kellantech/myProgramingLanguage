#ifndef _ST_H
#define _ST_H

class symbolTable{
  public:
  map<string,lType*>& tbl;
  symbolTable* parent;
  bool has_parent = false;
  symbolTable(map<string,lType*>& init_tbl)
          :tbl(init_tbl){}
  void setParent(symbolTable* par){
    parent = move(par);
    has_parent = true;
  }
  lType* get(string name){
    if(tbl.find(name) == tbl.end()){
      if (has_parent){ 
        return parent->get(name);
      }
      else{
        error("name "+ name +" not found");
        return nullptr;
      }
    }
    else {
      return move(tbl[name]);
    }
  }
  void set(string n,lType* v){
    tbl[n] = move(v);
  }
};

#endif
