#ifndef _ST_H
#define _ST_H

class symbolTable{
  public:
  map<string,lType*>& tbl;
  symbolTable* parent;
  bool has_parent = false;
  symbolTable(map<string,lType*>& t)
          :tbl(t){}
  void setParent(symbolTable* par){
    parent = move(par);
    has_parent = true;
  }
  lType* get(string nm){
    if(tbl.find(nm) == tbl.end()){
      if (has_parent){ 
        return parent->get(nm);
      }
      else{
        error("name "+ nm +" not found");
        return nullptr;
      }
    }
    else {
      return move(tbl[nm]);
    }
  }
  void set(string n,lType* v){
    tbl[n] = move(v);
  }
};

#endif
