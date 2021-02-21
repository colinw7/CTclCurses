#ifndef CTclCurses_H
#define CTclCurses_H

#include <CKeyType.h>
#include <string>
#include <vector>

class CTcl;

struct Tcl_Interp;
struct Tcl_Obj;

struct termios;

namespace CTclCurses {

class Widget;

//---

struct KeyData {
  CKeyType    type;
  std::string text;
};

struct MouseData {
};

//---

class App {
 public:
  using StringList = std::vector<std::string>;

 public:
  App(bool raw=false);
 ~App();

  //---

  CTcl *tcl() const { return tcl_; }

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

  int screenRows() const { return screenRows_; }
  int screenCols() const { return screenCols_; }

  //---

  bool init();

  void setVar(const std::string &name, const std::string &value);

  void exec(const std::string &file);

  void redraw();

  void loop();

  void prompt();

  //---

  void keyPress(const KeyData &);

  void mousePress  (const MouseData &) { }
  void mouseRelease(const MouseData &) { }

  //---

  void drawBox(int r1, int c1, int r2, int c2) const;

  void drawChar(int row, int col, char c) const;

  void moveTo(int row, int col) const;

  //---

  void processString(const std::string &str);
  bool processStringChar(unsigned char c);
  void processChar(unsigned char c);

  bool isRaw() const { return raw_; }
  bool setRaw(bool b);

  //---

  int numWidgets() const { return widgets_.size(); }

  void addWidget(Widget *w);

 private:
  static int clsProc        (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int cllProc        (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int moveProc       (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int drawTextProc   (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int styleProc      (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int bgColorProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int fgColorProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int drawBoxProc    (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int labelProc      (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int labelWidgetProc(void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int menuProc       (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int menuWidgetProc (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int checkProc      (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int checkWidgetProc(void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int winOpProc      (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int rawProc        (void *, Tcl_Interp *, int, const Tcl_Obj **);
  static int doneProc       (void *, Tcl_Interp *, int, const Tcl_Obj **);

  StringList getArgs(int objc, const Tcl_Obj **objv) const;

 private:
  using Widgets = std::vector<Widget *>;

  CTcl*       tcl_    { nullptr };
  bool        raw_    { false };
  bool        mouse_  { false };
  bool        loop_   { false };
  bool        prompt_ { false };
  int         icolor_ { 0 };
  bool        done_   { false };
  std::string result_;

  int screenRows_   { 0 };
  int screenCols_   { 0 };
  int screenHeight_ { 0 };
  int screenWidth_  { 0 };

  bool        inEscape_  { false };
  std::string escapeString_;

  struct termios *ios_ { nullptr };

  Widgets widgets_;
  Widgets focusWidgets_;
};

//---

class Widget {
 public:
  Widget(App *app, const std::string &name) :
   app_(app), name_(name) {
  }

  virtual ~Widget() { }

  App *app() const { return app_; }

  const std::string &name() const { return name_; }
  void setName(const std::string &s) { name_ = s; }

  virtual bool canFocus() const { return false; }

  bool hasFocus() const { return focus_; }
  void setFocus(bool b) { focus_ = b; }

  virtual void draw() const = 0;

  virtual void keyPress(const KeyData &) { }

 protected:
  App*        app_   { nullptr };
  std::string name_;
  bool        focus_ { false };
};

//---

class Label : public Widget {
 public:
  using StringList = std::vector<std::string>;

 public:
  Label(App *app, const std::string &name, int row, int col, const std::string &text) :
   Widget(app, name), row_(row), col_(col), text_(text) {
  }

  const std::string &text() const { return text_; }
  void setText(const std::string &s);

  void draw() const override;

 private:
  int         row_     { 0 };
  int         col_     { 0 };
  std::string text_;
};

//---

class Menu : public Widget {
 public:
  using StringList = std::vector<std::string>;

 public:
  Menu(App *app, const std::string &name, int row, int col, const StringList &strs);

  bool canFocus() const override { return true; }

  //---

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int col() const { return col_; }
  void setCol(int i) { col_ = i; }

  int width() const { return width_; }
  void setWidth(int i) { width_ = i; }

  int height() const { return height_; }
  void setHeight(int i) { height_ = i; }

  int currentInd() const { return current_; }
  void setCurrentInd(int i);

  //---

  std::string currentText() const;

  void draw() const override;

  void keyPress(const KeyData &data) override;

 private:
  int        row_     { 0 };
  int        col_     { 0 };
  int        width_   { -1 };
  int        height_  { -1 };
  StringList strs_;
  int        n_       { 0 };
  int        current_ { 0 };
//int        xOffset_ { 0 };
  int        yOffset_ { 0 };
};

//---

class Check : public Widget {
 public:
  using StringList = std::vector<std::string>;

 public:
  Check(App *app, const std::string &name, int row, int col);

  bool canFocus() const override { return true; }

  //---

  int row() const { return row_; }
  void setRow(int i) { row_ = i; }

  int col() const { return col_; }
  void setCol(int i) { col_ = i; }

  bool isChecked() const { return checked_; }
  void setChecked(bool b);

  //---

  void draw() const override;

  void keyPress(const KeyData &data) override;

 private:
  int  row_     { 0 };
  int  col_     { 0 };
  bool checked_ { false };
};

}

#endif
