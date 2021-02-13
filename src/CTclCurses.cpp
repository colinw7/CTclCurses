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
        std::cerr << "CTclCurses [-mouse] [-loop] [-prompt] [-raw] "
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

  CTclCurses app(raw);

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

  return 0;
}

CTclCurses::
CTclCurses(bool raw) :
 raw_(raw)
{
  if (raw_)
    setRaw(STDIN_FILENO);

  if (mouse_)
    COSRead::write(STDOUT_FILENO, CEscape::DECSET(1002));
}

CTclCurses::
~CTclCurses()
{
  if (mouse_)
    COSRead::write(STDOUT_FILENO, CEscape::DECRST(1002));

  if (raw_)
    resetRaw(STDIN_FILENO);

  if (result() != "")
    std::cout << result() << "\n";
}

bool
CTclCurses::
init()
{
  tcl_ = new CTcl();

  if (! tcl_->init())
    return false;

  tcl_->createObjCommand("cls",
    (CTcl::ObjCmdProc) &CTclCurses::clsProc    , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("cll",
    (CTcl::ObjCmdProc) &CTclCurses::cllProc    , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("move",
    (CTcl::ObjCmdProc) &CTclCurses::moveProc   , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("text",
    (CTcl::ObjCmdProc) &CTclCurses::textProc   , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("bgcolor",
    (CTcl::ObjCmdProc) &CTclCurses::bgColorProc, (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("fgcolor",
    (CTcl::ObjCmdProc) &CTclCurses::fgColorProc, (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("box",
    (CTcl::ObjCmdProc) &CTclCurses::boxProc    , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("winop",
    (CTcl::ObjCmdProc) &CTclCurses::winOpProc  , (CTcl::ObjCmdData) this);
  tcl_->createObjCommand("done",
    (CTcl::ObjCmdProc) &CTclCurses::doneProc   , (CTcl::ObjCmdData) this);

  tcl_->createAlias("echo" , "puts"   );
  tcl_->createAlias("color", "fgcolor");

  return true;
}

void
CTclCurses::
setVar(const std::string &name, const std::string &value)
{
  tcl_->createVar(name, value);
}

void
CTclCurses::
exec(const std::string &file)
{
  tcl_->eval("source \"" + file + "\"");
}

int
CTclCurses::
clsProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
  assert(th);

  auto args = th->getArgs(objc, objv);

  std::string op;

  if (args.size() > 0)
    op = args[0];

  if      (op == "below")
    COSRead::write(STDOUT_FILENO, CEscape::ED(0));
  else if (op == "above")
    COSRead::write(STDOUT_FILENO, CEscape::ED(1));
  else
    COSRead::write(STDOUT_FILENO, CEscape::ED(2)); // all

  return TCL_OK;
}

int
CTclCurses::
cllProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
  assert(th);

  auto args = th->getArgs(objc, objv);

  std::string op;

  if (args.size() > 0)
    op = args[0];

  if      (op == "right")
    COSRead::write(STDOUT_FILENO, CEscape::EL(0));
  else if (op == "left")
    COSRead::write(STDOUT_FILENO, CEscape::EL(1));
  else
    COSRead::write(STDOUT_FILENO, CEscape::EL(2)); // all

  return TCL_OK;
}

int
CTclCurses::
moveProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 2) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  COSRead::write(STDOUT_FILENO, CEscape::CUP(row, col));

  return TCL_OK;
}

int
CTclCurses::
textProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 1) return TCL_ERROR;

  COSRead::write(STDOUT_FILENO, args[0]);

  return TCL_OK;
}

int
CTclCurses::
bgColorProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
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
CTclCurses::
fgColorProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
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

int
CTclCurses::
boxProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
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
CTclCurses::
winOpProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  std::string op = args[0];

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
CTclCurses::
doneProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = (CTclCurses *) clientData;
  assert(th);

  auto args = th->getArgs(objc, objv);

  if (args.size() > 0)
    th->setResult(args[0]);

  th->setDone(true);

  return TCL_OK;
}

CTclCurses::StringList
CTclCurses::
getArgs(int objc, const Tcl_Obj **objv) const
{
  CTclCurses::StringList args;

  for (int i = 1; i < objc; ++i)
    args.push_back(CTclUtil::stringFromObj((Tcl_Obj *) objv[i]));

  return args;
}

void
CTclCurses::
loop()
{
  //redraw();

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

    //redraw();
  }
}

void
CTclCurses::
prompt()
{
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
CTclCurses::
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
CTclCurses::
processStringChar(unsigned char c)
{
  if (c == '') { // control backlash
    resetRaw(STDIN_FILENO);
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

    while (parse.isDigit()) {
      int i;

      parse.readInteger(&i);

      if (! parse.isChar(';'))
        break;

      parse.skipChar();
    }

    if (! parse.isAlpha())
      return false;

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

    inEscape_ = false;

    keyPress(data);

    return true;
  }
}

void
CTclCurses::
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
CTclCurses::
keyPress(const KeyData &data)
{
  tcl_->eval("keyPress " + std::to_string(int(data.type)) + " {" + data.text + "}");
}

//---

void
CTclCurses::
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
CTclCurses::
drawChar(int row, int col, char c) const
{
  COSRead::write(STDOUT_FILENO, CEscape::CUP(row, col));
  COSRead::write(STDOUT_FILENO, c);
}

//---

bool
CTclCurses::
setRaw(int fd)
{
  delete ios_;

  ios_ = new struct termios;

  COSPty::set_raw(fd, ios_);

  COSRead::write(STDOUT_FILENO, CEscape::DECSET(1049) + CEscape::DECRST(12,25));

  return true;
}

bool
CTclCurses::
resetRaw(int fd)
{
  if (tcsetattr(fd, TCSAFLUSH, ios_) < 0)
    return false;

  COSRead::write(STDOUT_FILENO, CEscape::DECRST(1049) + CEscape::DECSET(12,25));

  COSRead::write(STDOUT_FILENO, CEscape::SGR(0));

  delete ios_;

  ios_ = NULL;

  return true;
}
