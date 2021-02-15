#ifndef CTclCurses_H
#define CTclCurses_H

#include <CKeyType.h>
#include <string>
#include <vector>

class CTcl;

struct Tcl_Interp;
struct Tcl_Obj;

class CTclCurses {
 public:
  using StringList = std::vector<std::string>;

 public:
  CTclCurses(bool raw=false);
 ~CTclCurses();

  //---

  bool mouse() const { return mouse_; }
  void setMouse(bool b) { mouse_ = b; }

  bool isLoop() const { return loop_; }
  void setLoop(bool b) { loop_ = b; }

  bool isPrompt() const { return prompt_; }
  void setPrompt(bool b) { prompt_ = b; }

  int color() const { return icolor_; }
  void setColor(int i) { icolor_ = i; }

  bool isDone() const { return done_; }
  void setDone(bool b) { done_ = b; }

  const std::string &result() const { return result_; }
  void setResult(const std::string &v) { result_ = v; }

  //---

  bool init();

  void setVar(const std::string &name, const std::string &value);

  void exec(const std::string &file);

  void loop();

  void prompt();

  //---

  struct KeyData {
    CKeyType    type;
    std::string text;
  };

  struct MouseData {
  };

  void keyPress(const KeyData &);

  void mousePress  (const MouseData &) { }
  void mouseRelease(const MouseData &) { }

  //---

  void drawBox(int r1, int c1, int r2, int c2) const;

  void drawChar(int row, int col, char c) const;

  //---

  void processString(const std::string &str);
  bool processStringChar(unsigned char c);
  void processChar(unsigned char c);

  bool setRaw  ();
  bool resetRaw();

 private:
  CTcl *tcl() const { return tcl_; }

  static int clsProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int cllProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int moveProc   (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int textProc   (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int styleProc  (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int bgColorProc(void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int fgColorProc(void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int boxProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int winOpProc  (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int rawProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int doneProc   (void *, Tcl_Interp *, int, const Tcl_Obj **);

  StringList getArgs(int objc, const Tcl_Obj **objv) const;

 private:
  CTcl*       tcl_    { nullptr };
  bool        raw_    { false };
  bool        mouse_  { false };
  bool        loop_   { false };
  bool        prompt_ { false };
  int         icolor_ { 0 };
  bool        done_   { false };
  std::string result_;

  bool        inEscape_  { false };
  std::string escapeString_;

  struct termios *ios_ { nullptr };
};

#endif
