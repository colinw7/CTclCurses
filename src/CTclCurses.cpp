#include <CTclCurses.h>
#include <CTclUtil.h>
#include <CReadLine.h>
#include <CStrParse.h>
#include <CStrUtil.h>
#include <COSPty.h>
#include <COSRead.h>
#include <CEscape.h>

#include <map>
#include <termios.h>
#include <unistd.h>

int
main(int argc, char **argv)
{
  using NameValues = std::map<std::string, std::string>;

  std::string file;
  std::string ofile;

  bool mouse  = false;
  bool loop   = false;
  bool prompt = false;
  bool raw    = false;

  NameValues nameValues;

  for (int i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      auto arg = std::string(&argv[i][1]);

      if      (arg == "mouse")
        mouse = true;
      else if (arg == "loop")
        loop = true;
      else if (arg == "prompt")
        prompt = true;
      else if (arg == "raw")
        raw = true;
      else if (arg == "ofile") {
        ++i;

        if (i < argc)
          ofile = std::string(argv[i]);
      }
      else if (arg == "var") {
        ++i;

        if (i < argc) {
          auto nameValue = std::string(argv[i]);

          auto pos = nameValue.find('=');

          if (pos == std::string::npos) {
            std::cerr << "Invalid name value '" << nameValue << "'\n";
            continue;
          }

          auto lhs = nameValue.substr(0, pos);
          auto rhs = nameValue.substr(pos + 1);

          nameValues[lhs] = rhs;
        }
      }
      else if (arg == "h" || arg == "help") {
        std::cerr << "CTclCurses [-mouse] [-loop] [-prompt] [-raw] [-ofile <output_file>] "
                     "[var <name>=<value> ...] <file>\n";
        exit(0);
      }
      else {
        std::cerr << "Invalid opt '" << argv[i] << "'\n";
        continue;
      }
    }
    else {
      file = argv[i];
    }
  }

  CTclCurses::App app(raw);

  app.setLoop  (loop);
  app.setPrompt(prompt);

  app.init();

  for (const auto &nv : nameValues)
    app.setVar(nv.first, nv.second);

  if (file != "")
    app.exec(file);

  if      (app.isLoop())
    app.loop();
  else if (app.isPrompt())
    app.prompt();

  if (app.result() != "") {
    if (ofile != "") {
      if (ofile[0] != '/') {
        std::string homeDir = getenv("HOME");

        ofile = homeDir + "/" + ofile;
      }

      FILE *fp = fopen(ofile.c_str(), "w");
      if (! fp) return 1;

      fprintf(fp, "%s\n", app.result().c_str());

      fclose(fp);
    }
    else {
      app.setRaw(false);

      std::cout << app.result() << "\n";
    }
  }

  return 0;
}

//------

namespace CTclCurses {

App::
App(bool raw)
{
  if (raw)
    setRaw(true);

  if (mouse_)
    COSRead::write(STDOUT_FILENO, CEscape::DECSET(1002));
}

App::
~App()
{
  if (mouse_)
    COSRead::write(STDOUT_FILENO, CEscape::DECRST(1002));

  if (raw_)
    setRaw(false);
}

bool
App::
init()
{
  tcl_ = new CTcl();

  if (! tcl_->init())
    return false;

  tcl_->createObjCommand("cls",
    (CTcl::ObjCmdProc) &App::clsProc     , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("cll",
    (CTcl::ObjCmdProc) &App::cllProc     , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("move",
    (CTcl::ObjCmdProc) &App::moveProc    , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("draw_text",
    (CTcl::ObjCmdProc) &App::drawTextProc, (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("style",
    (CTcl::ObjCmdProc) &App::styleProc   , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("bgcolor",
    (CTcl::ObjCmdProc) &App::bgColorProc , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("fgcolor",
    (CTcl::ObjCmdProc) &App::fgColorProc , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("draw_box",
    (CTcl::ObjCmdProc) &App::drawBoxProc , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("label",
    (CTcl::ObjCmdProc) &App::labelProc   , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("menu",
    (CTcl::ObjCmdProc) &App::menuProc    , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("check",
    (CTcl::ObjCmdProc) &App::checkProc   , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("winop",
    (CTcl::ObjCmdProc) &App::winOpProc   , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("raw",
    (CTcl::ObjCmdProc) &App::rawProc     , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("done",
    (CTcl::ObjCmdProc) &App::doneProc    , (CTcl::ObjCmdData) this);

  tcl_->createAlias("echo" , "puts"   );
  tcl_->createAlias("color", "fgcolor");

  //---

  bool reset = false;

  if (! isRaw())
    reset = setRaw(true);

  //---

  if (CEscape::getWindowCharSize(&screenRows_, &screenCols_)) {
    tcl_->createVar("window_rows", screenRows_);
    tcl_->createVar("window_cols", screenCols_);
  }

  if (CEscape::getWindowPixelSize(&screenWidth_, &screenHeight_)) {
    tcl_->createVar("window_width" , screenWidth_ );
    tcl_->createVar("window_height", screenHeight_);
  }

  //---

  std::string res;

  tcl_->eval("proc redrawProc { } { }", res, /*showError*/false);

  //---

  if (reset)
    setRaw(false);

  return true;
}

void
App::
setVar(const std::string &name, const std::string &value)
{
  tcl_->createVar(name, value);
}

void
App::
exec(const std::string &file)
{
  std::string res;

  tcl_->eval("source \"" + file + "\"", res, /*showError*/true);
}

int
App::
clsProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);

  std::string op;

  if (args.size() > 0)
    op = CStrUtil::toLower(args[0]);

  if      (op == "below")
    COSRead::write(STDOUT_FILENO, CEscape::ED(0));
  else if (op == "above")
    COSRead::write(STDOUT_FILENO, CEscape::ED(1));
  else
    COSRead::write(STDOUT_FILENO, CEscape::ED(2)); // all

  return TCL_OK;
}

int
App::
cllProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);

  std::string op;

  if (args.size() > 0)
    op = CStrUtil::toLower(args[0]);

  if      (op == "right")
    COSRead::write(STDOUT_FILENO, CEscape::EL(0));
  else if (op == "left")
    COSRead::write(STDOUT_FILENO, CEscape::EL(1));
  else
    COSRead::write(STDOUT_FILENO, CEscape::EL(2)); // all

  return TCL_OK;
}

int
App::
moveProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 2) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  th->moveTo(row, col);

  return TCL_OK;
}

int
App::
drawTextProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 1) return TCL_ERROR;

  COSRead::write(STDOUT_FILENO, args[0]);

  return TCL_OK;
}

int
App::
styleProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 1) return TCL_ERROR;

  std::string op = CStrUtil::toLower(args[0]);

  if      (op == "bold")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(1));
  else if (op == "dim")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(2));
  else if (op == "italic")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(3));
  else if (op == "underscore")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(4));
  else if (op == "blink")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(5));
  else if (op == "invert")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(7));
  else if (op == "hidden")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(8));
  else if (op == "strikeout")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(9));
  else if (op == "doubleunderscore")
    COSRead::write(STDOUT_FILENO, CEscape::SGR(21));
  else
    COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  return TCL_OK;
}

int
App::
bgColorProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 1) return TCL_ERROR;

  int icolor = std::stoi(args[0]);

  th->setColor(icolor);

  if (icolor >= 0 && icolor <= 9)
    COSRead::write(STDOUT_FILENO, CEscape::SGR(40 + icolor));
  else
    COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  return TCL_OK;
}

int
App::
fgColorProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 1) return TCL_ERROR;

  int icolor = std::stoi(args[0]);

  th->setColor(icolor);

  if (icolor >= 0 && icolor <= 9)
    COSRead::write(STDOUT_FILENO, CEscape::SGR(30 + icolor));
  else
    COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  return TCL_OK;
}

//---

int
App::
labelProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 3) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  std::string widgetName = "label." + std::to_string(th->numWidgets() + 1);

  auto *label = new Label(th, widgetName, row, col, args[2]);

  th->tcl()->createObjCommand(widgetName,
    (CTcl::ObjCmdProc) &App::labelWidgetProc, (CTcl::ObjCmdData) label);

  th->addWidget(label);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
labelWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *label = static_cast<Label *>(clientData);
  assert(label);

  auto args = label->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if (args[1] == "text")
      label->app()->tcl()->setResult(label->text());
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if (args[1] == "text")
      label->setText(args[2]);
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
menuProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 3) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  CTcl::StringList strs;

  if (! th->tcl()->splitList(args[2], strs))
    return TCL_ERROR;

  std::string widgetName = "menu." + std::to_string(th->numWidgets() + 1);

  auto *menu = new Menu(th, widgetName, row, col, strs);

  th->tcl()->createObjCommand(widgetName,
    (CTcl::ObjCmdProc) &App::menuWidgetProc, (CTcl::ObjCmdData) menu);

  th->addWidget(menu);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
menuWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *menu = static_cast<Menu *>(clientData);
  assert(menu);

  auto args = menu->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if      (args[1] == "currentInd")
      menu->app()->tcl()->setResult(menu->currentInd());
    else if (args[1] == "currentText")
      menu->app()->tcl()->setResult(menu->currentText());
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if      (args[1] == "currentInd")
      menu->setCurrentInd(std::stoi(args[2]));
    else if (args[1] == "width")
      menu->setWidth(std::stoi(args[2]));
    else if (args[1] == "height")
      menu->setHeight(std::stoi(args[2]));
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
checkProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 2) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  std::string widgetName = "check." + std::to_string(th->numWidgets() + 1);

  auto *check = new Check(th, widgetName, row, col);

  th->tcl()->createObjCommand(widgetName,
    (CTcl::ObjCmdProc) &App::checkWidgetProc, (CTcl::ObjCmdData) check);

  th->addWidget(check);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
checkWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *check = static_cast<Check *>(clientData);
  assert(check);

  auto args = check->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if (args[1] == "checked")
      check->app()->tcl()->setResult(check->isChecked());
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if (args[1] == "checked")
      check->setChecked(std::stoi(args[2]));
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
drawBoxProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 4) return TCL_ERROR;

  int row1 = std::stoi(args[0]);
  int col1 = std::stoi(args[1]);
  int row2 = std::stoi(args[2]);
  int col2 = std::stoi(args[3]);

  th->drawBox(row1, col1, row2, col2);

  return TCL_OK;
}

int
App::
winOpProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  std::string op = CStrUtil::toLower(args[0]);

  if      (op == "deiconify")
    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::DEICONIFY));
  else if (op == "iconify")
    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::ICONIFY));
  else if (op == "move") {
    if (args.size() < 2) return TCL_ERROR;

    int x = std::stoi(args[1]);
    int y = std::stoi(args[2]);

    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::MOVE,
                   CStrUtil::toString(x), CStrUtil::toString(y)));
  }
  else if (op == "pixel_size") {
    if (args.size() < 2) return TCL_ERROR;

    int h = std::stoi(args[1]);
    int w = std::stoi(args[2]);

    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::PIXEL_SIZE,
                   CStrUtil::toString(h), CStrUtil::toString(w)));
  }
  else if (op == "raise")
    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::RAISE));
  else if (op == "lower")
    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::LOWER));
  else if (op == "refresh")
    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::REFRESH));
  else if (op == "char_size") {
    if (args.size() < 2) return TCL_ERROR;

    int h = std::stoi(args[1]);
    int w = std::stoi(args[2]);

    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::CHAR_SIZE,
                   CStrUtil::toString(h), CStrUtil::toString(w)));
  }
  else if (op == "maximize") {
    // restore 0, no-restore 1
    COSRead::write(STDOUT_FILENO, CEscape::windowOp(CEscape::WindowOp::MAXIMIZE, "0"));
  }
  else if (op == "icon_window_title") {
    if (args.size() < 2) return TCL_ERROR;

    COSRead::write(STDOUT_FILENO, CEscape::oscIconWindowTitle(args[1]));
  }
  else if (op == "icon_title") {
    if (args.size() < 2) return TCL_ERROR;

    COSRead::write(STDOUT_FILENO, CEscape::oscIconTitle(args[1]));
  }
  else if (op == "window_title") {
    if (args.size() < 2) return TCL_ERROR;

    COSRead::write(STDOUT_FILENO, CEscape::oscWindowTitle(args[1]));
  }
  else if (op == "window_prop") {
    if (args.size() < 2) return TCL_ERROR;

    COSRead::write(STDOUT_FILENO, CEscape::oscWindowProp(args[1]));
  }
  else
    TCL_ERROR;

  return TCL_OK;
}

int
App::
rawProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);

  std::string op;

  if (args.size() > 0)
    op = CStrUtil::toLower(args[0]);

  if (op == "" || op == "1") {
    th->setRaw(true);
  }
  else {
    th->setRaw(false);
  }

  return TCL_OK;
}
int
App::
doneProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);

  if (args.size() > 0)
    th->setResult(args[0]);

  th->setDone(true);

  return TCL_OK;
}

App::StringList
App::
getArgs(int objc, const Tcl_Obj **objv) const
{
  App::StringList args;

  for (int i = 1; i < objc; ++i)
    args.push_back(CTclUtil::stringFromObj((Tcl_Obj *) objv[i]));

  return args;
}

void
App::
loop()
{
  if (isDone()) return;

  redraw();

  while (true) {
    tcl_->processEvents();

    //---

    if (! COSRead::wait_read(STDIN_FILENO, 0, 100)) continue;

    std::string buffer;

    if (! COSRead::read(STDIN_FILENO, buffer)) continue;

    uint len = buffer.size();

    if (len == 0) continue;

    processString(buffer);

    if (isDone()) break;

    redraw();
  }
}

void
App::
redraw()
{
  COSRead::write(STDOUT_FILENO, CEscape::ED(2)); // all

  std::string res;

  tcl_->eval("redrawProc", res, /*showError*/true);

  for (const auto &w : widgets_)
    w->draw();
}

void
App::
prompt()
{
  if (isDone()) return;

  CReadLine readline;

  readline.setAutoHistory(true);

  std::string line;

  while (true) {
    tcl_->processEvents();

    //---

    readline.setPrompt("> ");

    line = readline.readLine();

    std::string res;

    tcl_->eval(line.c_str(), res, /*showError*/true);

    if (isDone()) break;
  }
}

void
App::
processString(const std::string &str)
{
  inEscape_ = false;

  uint i   = 0;
  uint len = str.size();

  //---

#if 0
  static int pressButton = 0;

  // process mouse (only if mouse enabled ?)
  int  button, col, row;
  bool release;

  if (len == 6 && CEscape::parseMouse(str, &button, &col, &row, &release)) {
    int rows, cols, width, height;

    if (! CEscape::getWindowCharSize(&rows, &cols))
      return;

    if (! CEscape::getWindowPixelSize(&width, &height))
      return;

    if (rows <= 0) rows = 1;
    if (cols <= 0) cols = 1;

    int cw = width /cols;
    int ch = height/rows;

    int x1 = (col - 1)*cw + cw/2;
    int y1 = (row - 1)*ch + ch/2;

    CIPoint2D pos(x1, y1);

    if (! release) {
      CMouseEvent event(pos, (CMouseButton) (button + 1));

      mousePress(event);

      pressButton = button;
    }
    else {
      CMouseEvent event(pos, (CMouseButton) (pressButton + 1));

      mouseRelease(event);
    }

    return;
  }
#endif

  //---

  // skip status report
  if (len > 8 && str[0] == '' && str[1] == '[' && str[len - 1] == 't')
    return;

  //---

  for (uint pos = 0; pos < len; ++pos) {
    if (processStringChar(str[pos]))
      i = pos + 1;

    if (done_) return;
  }

  if (i < len) {
    for (uint pos = i; pos < len; ++pos) {
      processChar(str[pos]);

      if (done_) return;
    }
  }
}

bool
App::
processStringChar(unsigned char c)
{
  if (c == '') { // control backlash
    if (raw_)
      setRaw(false);

    exit(1);
  }

  KeyData data;

  if (! inEscape_) {
    if (c == '\033') {
      inEscape_     = true;
      escapeString_ = std::string((char *) &c, 1);
      return false;
    }

    processChar(c);

    return true;
  }
  else {
    escapeString_ += c;

    CStrParse parse(escapeString_);

    if (! parse.isChar(''))
      return false;

    parse.skipChar();

    if (! parse.isChar('['))
      return false;

    parse.skipChar();

    int n = 0;

    while (parse.isDigit()) {
      int i;

      parse.readInteger(&i);

      n += 10*n + i;

      if (! parse.isChar(';'))
        break;

      parse.skipChar();
    }

    if (parse.isAlpha()) {
      char c1 = parse.getCharAt();

      if      (c1 == 'A') { // up
        data.type = CKEY_TYPE_Up;
        data.text = "up";
      }
      else if (c1 == 'B') { // down
        data.type = CKEY_TYPE_Down;
        data.text = "down";
      }
      else if (c1 == 'C') { // right
        data.type = CKEY_TYPE_Right;
        data.text = "right";
      }
      else if (c1 == 'D') { // left
        data.type = CKEY_TYPE_Left;
        data.text = "left";
      }
      else if (c1 == 'F') { // end
        data.type = CKEY_TYPE_End;
        data.text = "end";
      }
      else if (c1 == 'H') { // home
        data.type = CKEY_TYPE_Home;
        data.text = "home";
      }
    }
    else
      return false;

    inEscape_ = false;

    keyPress(data);

    return true;
  }
}

void
App::
processChar(unsigned char c)
{
  KeyData data;

  switch (c) {
    case 0     : { data.type = CKEY_TYPE_NUL         ; break; }
    case ''  : { data.type = CKEY_TYPE_SOH         ; break; }
    case ''  : { data.type = CKEY_TYPE_STX         ; break; }
    case ''  : { data.type = CKEY_TYPE_ETX         ; break; }
    case ''  : { data.type = CKEY_TYPE_EOT         ; break; }
    case ''  : { data.type = CKEY_TYPE_ENQ         ; break; }
    case ''  : { data.type = CKEY_TYPE_ACK         ; break; }
    case ''  : { data.type = CKEY_TYPE_BEL         ; break; }
    case ''  : { data.type = CKEY_TYPE_BackSpace   ; data.text = "backspace"; break; }
    case '\011': { data.type = CKEY_TYPE_TAB         ; data.text = "tab"      ; break; }
    case '\012': { data.type = CKEY_TYPE_LineFeed    ; data.text = "lf"       ; break; }
    case ''  : { data.type = CKEY_TYPE_Clear       ; data.text = "clear"    ; break; }
    case ''  : { data.type = CKEY_TYPE_FF          ; data.text = "ff"       ; break; }
    case '\015': { data.type = CKEY_TYPE_Return      ; data.text = "return"   ; break; }
    case ''  : { data.type = CKEY_TYPE_SO          ; break; }
    case ''  : { data.type = CKEY_TYPE_SI          ; break; }
    case ''  : { data.type = CKEY_TYPE_DLE         ; break; }
    case '\021': { data.type = CKEY_TYPE_DC1         ; break; }
    case ''  : { data.type = CKEY_TYPE_DC2         ; break; }
    case '\023': { data.type = CKEY_TYPE_Pause       ; break; }
    case ''  : { data.type = CKEY_TYPE_Scroll_Lock ; break; }
    case ''  : { data.type = CKEY_TYPE_Sys_Req     ; break; }
    case ''  : { data.type = CKEY_TYPE_SYN         ; break; }
    case ''  : { data.type = CKEY_TYPE_ETB         ; break; }
    case ''  : { data.type = CKEY_TYPE_CAN         ; break; }
    case ''  : { data.type = CKEY_TYPE_EM          ; break; }
    case ''  : { data.type = CKEY_TYPE_SUB         ; break; }
    case '\033': { data.type = CKEY_TYPE_Escape      ; data.text = "escape"; break; }
    case '\034': { data.type = CKEY_TYPE_FS          ; break; }
    case '\035': { data.type = CKEY_TYPE_GS          ; break; }
    case '\036': { data.type = CKEY_TYPE_RS          ; break; }
    case '\037': { data.type = CKEY_TYPE_US          ; break; }

    // auto named ' ' to '}'
    case ' '   : { data.type = CKEY_TYPE_Space       ; break; }
    case '!'   : { data.type = CKEY_TYPE_Exclam      ; break; }
    case '\"'  : { data.type = CKEY_TYPE_QuoteDbl    ; break; }
    case '#'   : { data.type = CKEY_TYPE_NumberSign  ; break; }
    case '$'   : { data.type = CKEY_TYPE_Dollar      ; break; }
    case '%'   : { data.type = CKEY_TYPE_Percent     ; break; }
    case '&'   : { data.type = CKEY_TYPE_Ampersand   ; break; }
    case '\''  : { data.type = CKEY_TYPE_Apostrophe  ; break; }
    case '('   : { data.type = CKEY_TYPE_ParenLeft   ; break; }
    case ')'   : { data.type = CKEY_TYPE_ParenRight  ; break; }
    case '*'   : { data.type = CKEY_TYPE_Asterisk    ; break; }
    case '+'   : { data.type = CKEY_TYPE_Plus        ; break; }
    case ','   : { data.type = CKEY_TYPE_Comma       ; break; }
    case '-'   : { data.type = CKEY_TYPE_Minus       ; break; }
    case '.'   : { data.type = CKEY_TYPE_Period      ; break; }
    case '/'   : { data.type = CKEY_TYPE_Slash       ; break; }
    case '0'   : { data.type = CKEY_TYPE_0           ; break; }
    case '1'   : { data.type = CKEY_TYPE_1           ; break; }
    case '2'   : { data.type = CKEY_TYPE_2           ; break; }
    case '3'   : { data.type = CKEY_TYPE_3           ; break; }
    case '4'   : { data.type = CKEY_TYPE_4           ; break; }
    case '5'   : { data.type = CKEY_TYPE_5           ; break; }
    case '6'   : { data.type = CKEY_TYPE_6           ; break; }
    case '7'   : { data.type = CKEY_TYPE_7           ; break; }
    case '8'   : { data.type = CKEY_TYPE_8           ; break; }
    case '9'   : { data.type = CKEY_TYPE_9           ; break; }
    case ':'   : { data.type = CKEY_TYPE_Colon       ; break; }
    case ';'   : { data.type = CKEY_TYPE_Semicolon   ; break; }
    case '<'   : { data.type = CKEY_TYPE_Less        ; break; }
    case '='   : { data.type = CKEY_TYPE_Equal       ; break; }
    case '>'   : { data.type = CKEY_TYPE_Greater     ; break; }
    case '?'   : { data.type = CKEY_TYPE_Question    ; break; }
    case '@'   : { data.type = CKEY_TYPE_At          ; break; }
    case 'A'   : { data.type = CKEY_TYPE_A           ; break; }
    case 'B'   : { data.type = CKEY_TYPE_B           ; break; }
    case 'C'   : { data.type = CKEY_TYPE_C           ; break; }
    case 'D'   : { data.type = CKEY_TYPE_D           ; break; }
    case 'E'   : { data.type = CKEY_TYPE_E           ; break; }
    case 'F'   : { data.type = CKEY_TYPE_F           ; break; }
    case 'G'   : { data.type = CKEY_TYPE_G           ; break; }
    case 'H'   : { data.type = CKEY_TYPE_H           ; break; }
    case 'I'   : { data.type = CKEY_TYPE_I           ; break; }
    case 'J'   : { data.type = CKEY_TYPE_J           ; break; }
    case 'K'   : { data.type = CKEY_TYPE_K           ; break; }
    case 'L'   : { data.type = CKEY_TYPE_L           ; break; }
    case 'M'   : { data.type = CKEY_TYPE_M           ; break; }
    case 'N'   : { data.type = CKEY_TYPE_N           ; break; }
    case 'O'   : { data.type = CKEY_TYPE_O           ; break; }
    case 'P'   : { data.type = CKEY_TYPE_P           ; break; }
    case 'Q'   : { data.type = CKEY_TYPE_Q           ; break; }
    case 'R'   : { data.type = CKEY_TYPE_R           ; break; }
    case 'S'   : { data.type = CKEY_TYPE_S           ; break; }
    case 'T'   : { data.type = CKEY_TYPE_T           ; break; }
    case 'U'   : { data.type = CKEY_TYPE_U           ; break; }
    case 'V'   : { data.type = CKEY_TYPE_V           ; break; }
    case 'W'   : { data.type = CKEY_TYPE_W           ; break; }
    case 'X'   : { data.type = CKEY_TYPE_X           ; break; }
    case 'Y'   : { data.type = CKEY_TYPE_Y           ; break; }
    case 'Z'   : { data.type = CKEY_TYPE_Z           ; break; }
    case '['   : { data.type = CKEY_TYPE_BracketLeft ; break; }
    case '\\'  : { data.type = CKEY_TYPE_Backslash   ; break; }
    case ']'   : { data.type = CKEY_TYPE_BracketRight; break; }
    case '~'   : { data.type = CKEY_TYPE_AsciiCircum ; break; }
    case '_'   : { data.type = CKEY_TYPE_Underscore  ; break; }
    case '`'   : { data.type = CKEY_TYPE_QuoteLeft   ; break; }
    case 'a'   : { data.type = CKEY_TYPE_a           ; break; }
    case 'b'   : { data.type = CKEY_TYPE_b           ; break; }
    case 'c'   : { data.type = CKEY_TYPE_c           ; break; }
    case 'd'   : { data.type = CKEY_TYPE_d           ; break; }
    case 'e'   : { data.type = CKEY_TYPE_e           ; break; }
    case 'f'   : { data.type = CKEY_TYPE_f           ; break; }
    case 'g'   : { data.type = CKEY_TYPE_g           ; break; }
    case 'h'   : { data.type = CKEY_TYPE_h           ; break; }
    case 'i'   : { data.type = CKEY_TYPE_i           ; break; }
    case 'j'   : { data.type = CKEY_TYPE_j           ; break; }
    case 'k'   : { data.type = CKEY_TYPE_k           ; break; }
    case 'l'   : { data.type = CKEY_TYPE_l           ; break; }
    case 'm'   : { data.type = CKEY_TYPE_m           ; break; }
    case 'n'   : { data.type = CKEY_TYPE_n           ; break; }
    case 'o'   : { data.type = CKEY_TYPE_o           ; break; }
    case 'p'   : { data.type = CKEY_TYPE_p           ; break; }
    case 'q'   : { data.type = CKEY_TYPE_q           ; break; }
    case 'r'   : { data.type = CKEY_TYPE_r           ; break; }
    case 's'   : { data.type = CKEY_TYPE_s           ; break; }
    case 't'   : { data.type = CKEY_TYPE_t           ; break; }
    case 'u'   : { data.type = CKEY_TYPE_u           ; break; }
    case 'v'   : { data.type = CKEY_TYPE_v           ; break; }
    case 'w'   : { data.type = CKEY_TYPE_w           ; break; }
    case 'x'   : { data.type = CKEY_TYPE_x           ; break; }
    case 'y'   : { data.type = CKEY_TYPE_y           ; break; }
    case 'z'   : { data.type = CKEY_TYPE_z           ; break; }
    case '{'   : { data.type = CKEY_TYPE_BraceLeft   ; break; }
    case '|'   : { data.type = CKEY_TYPE_Bar         ; break; }
    case '}'   : { data.type = CKEY_TYPE_BraceRight  ; break; }

    case 0x7f  : { data.type = CKEY_TYPE_DEL         ; break; }
    default    : {                                     break; }
  }

  if (c >= ' ' && c <= '}')
    data.text = std::string((char *) &c, 1);

  keyPress(data);
}

void
App::
keyPress(const KeyData &data)
{
  if      (data.text == "tab") {
    int pos = 0;

    for (auto *w : focusWidgets_) {
      if (w->hasFocus())
        break;

      ++pos;
    }

    int focusPos = pos;

    ++focusPos;

    if (focusPos >= int(focusWidgets_.size()))
      focusPos = 0;

    pos = 0;

    for (auto *w : focusWidgets_) {
      w->setFocus(pos == focusPos);

      ++pos;
    }

    redraw();
  }
  else if (data.text == "escape") {
    setDone(true);

    return;
  }

  for (auto *w : focusWidgets_)
    w->keyPress(data);

  std::string res;

  tcl_->eval("keyPressProc " + std::to_string(int(data.type)) + " {" + data.text + "}",
              res, /*showError*/true);
}

//---

void
App::
drawBox(int r1, int c1, int r2, int c2) const
{
  drawChar(r1, c1, '+');
  drawChar(r1, c2, '+');
  drawChar(r2, c1, '+');
  drawChar(r2, c2, '+');

  for (int r = r1 + 1; r <= r2 - 1; ++r) {
    drawChar(r, c1, '|');
    drawChar(r, c2, '|');
  }

  for (int c = c1 + 1; c <= c2 - 1; ++c) {
    drawChar(r1, c, '-');
    drawChar(r2, c, '-');
  }
}

void
App::
drawChar(int row, int col, char c) const
{
  moveTo(row, col);

  COSRead::write(STDOUT_FILENO, c);
}

void
App::
moveTo(int row, int col) const
{
  COSRead::write(STDOUT_FILENO, CEscape::CUP(row, col));
}

//---

bool
App::
setRaw(bool b)
{
  if (b) {
    if (! raw_) {
      delete ios_;

      ios_ = new struct termios;

      COSPty::set_raw(STDOUT_FILENO, ios_);

      COSRead::write(STDOUT_FILENO, CEscape::DECSET(1049) + CEscape::DECRST(12,25));

      raw_ = true;
    }
  }
  else {
    if (raw_) {
      if (tcsetattr(STDOUT_FILENO, TCSAFLUSH, ios_) < 0)
        return false;

      COSRead::write(STDOUT_FILENO, CEscape::DECRST(1049) + CEscape::DECSET(12,25));

      COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

      delete ios_;

      ios_ = nullptr;

      raw_ = false;
    }
  }

  return true;
}

//---

void
App::
addWidget(Widget *w)
{
  widgets_.push_back(w);

  if (w->canFocus()) {
    if (focusWidgets_.empty())
      w->setFocus(true);

    focusWidgets_.push_back(w);
  }
}

//------

void
Label::
setText(const std::string &str)
{
  text_ = str;

  app_->redraw();
}

void
Label::
draw() const
{
  app_->moveTo(row_, col_);

  COSRead::write(STDOUT_FILENO, text_);
}

//---

Menu::
Menu(App *app, const std::string &name, int row, int col, const StringList &strs) :
 Widget(app, name), row_(row), col_(col), strs_(strs)
{
  width_ = 0;

  for (const auto &str : strs_)
    width_ = std::max(width_, int(str.size()));

  height_ = strs_.size();

  n_ = strs_.size();
}

void
Menu::
setCurrentInd(int i)
{
  if (i < 0 || i >= int(strs_.size()))
    return;

  current_ = i;

  app_->redraw();
}

std::string
Menu::
currentText() const
{
  if (current_ < 0 || current_ >= int(strs_.size()))
    return "";

  return strs_[current_];
}

void
Menu::
draw() const
{
  int focusColor = 5;

  if (hasFocus())
    COSRead::write(STDOUT_FILENO, CEscape::SGR(30 + focusColor));
  else
    COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  app_->drawBox(row_, col_, row_ + height_ + 1, col_ + width_ + 5);

  COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  int cursorColor = 3;

  int y = 0;

  for (int i = 0; i < n_; ++i) {
    if (i < -yOffset_)
      continue;

    if (y >= height_)
      break;

    const auto &str = strs_[y - yOffset_];

    app_->moveTo(y + row_ + 1, col_ + 1);

    if (i == current_) {
      if (hasFocus())
        COSRead::write(STDOUT_FILENO, CEscape::SGR(30 + cursorColor));
      else
        COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

      COSRead::write(STDOUT_FILENO, "->");

      COSRead::write(STDOUT_FILENO, CEscape::SGR(0));
    }
    else
      COSRead::write(STDOUT_FILENO, "  ");

    app_->moveTo(y + row_ + 1, col_ + 4);

    COSRead::write(STDOUT_FILENO, str);

    ++y;
  }

  app_->moveTo(y + row_ + 3, 0);
}

void
Menu::
keyPress(const KeyData &data)
{
  if (! hasFocus())
    return;

  std::string res;

  if      (data.text == "home") {
    if (current_ <= 0) return;

    current_ = 0;

    app_->redraw();

    app_->tcl()->eval("widgetChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "end") {
    if (current_ >= int(n_ - 1)) return;

    current_ = n_ - 1;

    app_->redraw();

    app_->tcl()->eval("widgetChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "down") {
    if (current_ >= int(n_ - 1)) return;

    ++current_;

    if (current_ + yOffset_ + 1 > height_) {
      if (yOffset_ > height_ - n_)
        --yOffset_;
    }

    app_->redraw();

    app_->tcl()->eval("widgetChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "up") {
    if (current_ <= 0) return;

    --current_;

    if (current_ + yOffset_ - 1 < 0) {
      if (yOffset_ < 0)
        ++yOffset_;
    }

    app_->redraw();

    app_->tcl()->eval("widgetChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "return") {
    //if (current_ >= 0 && current_ <= int(n_ - 1))
    //  app_->setResult(strs_[current_]);
    //app_->setDone(true);

    app_->tcl()->eval("widgetExecProc {" + name() + "}", res, /*showError*/true);
  }
}

//---

Check::
Check(App *app, const std::string &name, int row, int col) :
 Widget(app, name), row_(row), col_(col)
{
}

void
Check::
setChecked(bool b)
{
  checked_ = b;

  app_->redraw();
}

void
Check::
draw() const
{
  int focusColor = 5;

  if (hasFocus())
    COSRead::write(STDOUT_FILENO, CEscape::SGR(30 + focusColor));
  else
    COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  app_->drawBox(row_, col_, row_ + 2, col_ + 2);

  COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  app_->moveTo(row_ + 1, col_ + 1);

  if (checked_)
    COSRead::write(STDOUT_FILENO, "x");
}

void
Check::
keyPress(const KeyData &data)
{
  if (! hasFocus())
    return;

  std::string res;

  if (data.text == "return" || data.text == " ") {
    checked_ = ! checked_;

    app_->tcl()->eval("widgetChangedProc {" + name() + "}", res, /*showError*/true);

    app_->redraw();
  }
}

//------

}
