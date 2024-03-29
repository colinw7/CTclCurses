#include <CTclCurses.h>
#include <CReadLine.h>
#include <CImageLib.h>
#include <CStrParse.h>
#include <CStrUtil.h>
#include <COSPty.h>
#include <COSRead.h>
#include <CEscape.h>
#include <CRGBName.h>
#include <CRGBUtil.h>

#include <map>
#include <termios.h>
#include <unistd.h>

namespace {
  int hexStrToInt(const std::string &str) {
    uint i;
    sscanf(str.c_str(), "%x", &i);
    return i;
  }

  void nameToRgb(const std::string &name, int &r, int &g, int &b) {
    assert(name[0] == '#');
    r = hexStrToInt(name.substr(1, 2));
    g = hexStrToInt(name.substr(3, 2));
    b = hexStrToInt(name.substr(5, 2));
  }

  const std::vector<std::string> &getNamedColors(const std::string &name) {
    if      (name == "red") {
      static std::vector<std::string> colors = {
        "#fde0dc", "#f9bdbb", "#f69988", "#f36c60", "#e84e40", "#e51c23", "#dd191d",
        "#d01716", "#c41411", "#b0120a", "#ff7997", "#ff5177", "#ff2d6f", "#e00032" };
      return colors;
    }
    else if (name == "pink") {
      static std::vector<std::string> colors = {
        "#fce4ec", "#f8bbd0", "#f48fb1", "#f06292", "#ec407a", "#e91e63", "#d81b60",
        "#c2185b", "#ad1457", "#880e4f", "#ff80ab", "#ff4081", "#f50057", "#c51162" };
      return colors;
    }
    else if (name == "purple") {
      static std::vector<std::string> colors = {
        "#f3e5f5", "#e1bee7", "#ce93d8", "#ba68c8", "#ab47bc", "#9c27b0", "#8e24aa",
        "#7b1fa2", "#6a1b9a", "#4a148c", "#ea80fc", "#e040fb", "#d500f9", "#aa00ff" };
      return colors;
    }
    else if (name == "deep_purple") {
      static std::vector<std::string> colors = {
        "#ede7f6", "#d1c4e9", "#b39ddb", "#9575cd", "#7e57c2", "#673ab7", "#5e35b1",
        "#512da8", "#4527a0", "#311b92", "#b388ff", "#7c4dff", "#651fff", "#6200ea" };
      return colors;
    }
    else if (name == "indigo") {
      static std::vector<std::string> colors = {
        "#e8eaf6", "#c5cae9", "#9fa8da", "#7986cb", "#5c6bc0", "#3f51b5", "#3949ab",
        "#303f9f", "#283593", "#1a237e", "#8c9eff", "#536dfe", "#3d5afe", "#304ffe" };
      return colors;
    }
    else if (name == "blue") {
      static std::vector<std::string> colors = {
        "#e7e9fd", "#d0d9ff", "#afbfff", "#91a7ff", "#738ffe", "#5677fc", "#4e6cef",
        "#455ede", "#3b50ce", "#2a36b1", "#a6baff", "#6889ff", "#4d73ff", "#4d69ff" };
      return colors;
    }
    else if (name == "light_blue") {
      static std::vector<std::string> colors = {
        "#e1f5fe", "#b3e5fc", "#81d4fa", "#4fc3f7", "#29b6f6", "#03a9f4", "#039be5",
        "#0288d1", "#0277bd", "#01579b", "#80d8ff", "#40c4ff", "#00b0ff", "#0091ea" };
      return colors;
    }
    else if (name == "cyan") {
      static std::vector<std::string> colors = {
        "#e0f7fa", "#b2ebf2", "#80deea", "#4dd0e1", "#26c6da", "#00bcd4", "#00acc1",
        "#0097a7", "#00838f", "#006064", "#84ffff", "#18ffff", "#00e5ff", "#00b8d4" };
      return colors;
    }
    else if (name == "teal") {
      static std::vector<std::string> colors = {
        "#e0f2f1", "#b2dfdb", "#80cbc4", "#4db6ac", "#26a69a", "#009688", "#00897b",
        "#00796b", "#00695c", "#004d40", "#a7ffeb", "#64ffda", "#1de9b6", "#00bfa5" };
      return colors;
    }
    else if (name == "green") {
      static std::vector<std::string> colors = {
        "#d0f8ce", "#a3e9a4", "#72d572", "#42bd41", "#2baf2b", "#259b24", "#0a8f08",
        "#0a7e07", "#056f00", "#0d5302", "#a2f78d", "#5af158", "#14e715", "#12c700" };

      return colors;
    }
    else if (name == "light_green") {
      static std::vector<std::string> colors = {
        "#f1f8e9", "#dcedc8", "#c5e1a5", "#aed581", "#9ccc65", "#8bc34a", "#7cb342",
        "#689f38", "#558b2f", "#33691e", "#ccff90", "#b2ff59", "#76ff03", "#64dd17" };
      return colors;
    }
    else if (name == "lime") {
      static std::vector<std::string> colors = {
        "#f9fbe7", "#f0f4c3", "#e6ee9c", "#dce775", "#d4e157", "#cddc39", "#c0ca33",
        "#afb42b", "#9e9d24", "#827717", "#f4ff81", "#eeff41", "#c6ff00", "#aeea00" };
      return colors;
    }
    else if (name == "yellow") {
      static std::vector<std::string> colors = {
        "#fffde7", "#fff9c4", "#fff59d", "#fff176", "#ffee58", "#ffeb3b", "#fdd835",
        "#fbc02d", "#f9a825", "#f57f17", "#ffff8d", "#ffff00", "#ffea00", "#ffd600" };
      return colors;
    }
    else if (name == "amber") {
      static std::vector<std::string> colors = {
        "#fff8e1", "#ffecb3", "#ffe082", "#ffd54f", "#ffca28", "#ffc107", "#ffb300",
        "#ffa000", "#ff8f00", "#ff6f00", "#ffe57f", "#ffd740", "#ffc400", "#ffab00" };
      return colors;
    }
    else if (name == "orange") {
      static std::vector<std::string> colors = {
        "#fff3e0", "#ffe0b2", "#ffcc80", "#ffb74d", "#ffa726", "#ff9800", "#fb8c00",
        "#f57c00", "#ef6c00", "#e65100", "#ffd180", "#ffab40", "#ff9100", "#ff6d00" };
      return colors;
    }
    else if (name == "deep_orange") {
      static std::vector<std::string> colors = {
        "#fbe9e7", "#ffccbc", "#ffab91", "#ff8a65", "#ff7043", "#ff5722", "#f4511e",
        "#e64a19", "#d84315", "#bf360c", "#ff9e80", "#ff6e40", "#ff3d00", "#dd2c00" };
      return colors;
    }
    else if (name == "brown") {
      static std::vector<std::string> colors = {
        "#efebe9", "#d7ccc8", "#bcaaa4", "#a1887f", "#8d6e63", "#795548", "#6d4c41",
        "#5d4037", "#4e342e", "#3e2723" };
      return colors;
    }
    else if (name == "grey") {
      static std::vector<std::string> colors = {
        "#ffffff", "#fafafa", "#f5f5f5", "#eeeeee", "#e0e0e0", "#bdbdbd", "#9e9e9e",
        "#757575", "#616161", "#424242", "#212121", "#000000" };
      return colors;
    }
    else if (name == "blue_grey") {
      static std::vector<std::string> colors = {
        "#eceff1", "#cfd8dc", "#b0bec5", "#90a4ae", "#78909c", "#607d8b", "#546e7a",
        "#455a64", "#37474f", "#263238" };
      return colors;
    }

    static std::vector<std::string> no_colors;

    return no_colors;
  }

  bool colorSetColor(const std::string &name, int i, int &r, int &g, int &b) {
    const auto &colors = getNamedColors(name);

    if (i < 0 || i >= int(colors.size()))
      return false;

    nameToRgb(colors[size_t(i)], r, g, b);

    return true;
  }
}

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

          auto lhs = nameValue.substr(0, size_t(pos));
          auto rhs = nameValue.substr(size_t(pos + 1));

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
  app.setMouse (mouse);

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

  app.term();

  return 0;
}

//------

namespace CTclCurses {

void
Tcl::
outputError(const std::string &msg)
{
  app_->outputError(msg);
}

void
Tcl::
handleRead(const char *name)
{
  app_->handleRead(name);
}

//---

App::
App(bool raw)
{
  //CEscape::setLogResult(true);

  CEscape::setReadResultTime(0, 500000);

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

  //CEscape::setLogResult(false);
}

bool
App::
init()
{
  tcl_ = new Tcl(this);

  if (! tcl_->init())
    return false;

  tcl_->createObjCommand("cls",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::clsProc     ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("cll",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::cllProc     ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("move",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::moveProc    ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("style",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::styleProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("bgcolor",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::bgColorProc ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("fgcolor",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::fgColorProc ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("draw_box",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::drawBoxProc ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("draw_text",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::drawTextProc), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("tlabel",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::labelProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("tlist",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::listProc    ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("ttable",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::tableProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("tcheck",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::checkProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("tinput",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::inputProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("tbox",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::boxProc     ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("timage",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::imageProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("winop",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::winOpProc   ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("ttystate",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::ttyStateProc), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("raw",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::rawProc     ), static_cast<Tcl::ObjCmdData>(this));
  tcl_->createObjCommand("done",
    reinterpret_cast<Tcl::ObjCmdProc>(&App::doneProc    ), static_cast<Tcl::ObjCmdData>(this));

  tcl_->createAlias("echo" , "puts"   );
  tcl_->createAlias("color", "fgcolor");

  //---

  bool reset = false;

  if (! isRaw())
    reset = setRaw(true);

  //---

  std::vector<int> ivals; ivals.push_back(0); ivals.push_back(0);

  tcl_->createVar("window_pos"       , ivals);
  tcl_->createVar("window_char_size" , ivals);
  tcl_->createVar("window_pixel_size", ivals);

  tcl_->traceVar("window_pos"       );
  tcl_->traceVar("window_char_size" );
  tcl_->traceVar("window_pixel_size");

  //---

  std::string res;

  tcl_->eval("proc redrawProc { } { }", res, /*showError*/true);
  tcl_->eval("proc keyPressProc { args } { }", res, /*showError*/true);
  tcl_->eval("proc mousePressProc { args } { }", res, /*showError*/true);
  tcl_->eval("proc mouseReleaseProc { args } { }", res, /*showError*/true);

  //---

  if (reset)
    setRaw(false);

  return true;
}

void
App::
term()
{
  setRaw(false);

  if (errorMsg_ != "")
    std::cerr << "Errors:\n" << errorMsg_ << "\n";
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

  if (args[0] == "&solid;")
    COSRead::write(STDOUT_FILENO, "\u2588");
  else
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
    th->clearStyle();

  return TCL_OK;
}

int
App::
bgColorProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if      (args[0] == "rgb") {
    if (args.size() < 4) return TCL_ERROR;

    int r = std::stoi(args[1]);
    int g = std::stoi(args[2]);
    int b = std::stoi(args[3]);

    COSRead::write(STDOUT_FILENO, CEscape::SGR_bg(r, g, b));
  }
  else if (args[0] == "palette") {
    if (args.size() < 2) return TCL_ERROR;

    int n = std::stoi(args[1]);

    COSRead::write(STDOUT_FILENO, CEscape::SGR_bg(n));
  }
  else if (args[0] == "bright") {
    if (args.size() != 2) return TCL_ERROR;

    int icolor = std::stoi(args[1]);

    if (icolor >= 0 && icolor <= 9)
      COSRead::write(STDOUT_FILENO, CEscape::SGR(90 + icolor));
  }
  else if (args[0] == "colorset") {
    if (args.size() != 3) return TCL_ERROR;

    auto name = args[1];
    auto ind  = std::stoi(args[2]);

    int r, g, b;

    if (colorSetColor(name, ind, r, g, b))
      COSRead::write(STDOUT_FILENO, CEscape::SGR_bg(r, g, b));
  }
  else {
    if (args.size() != 1) return TCL_ERROR;

    int icolor = std::stoi(args[0]);

    th->setColor(icolor);

    if (icolor >= 0 && icolor <= 9)
      th->bgIColor(icolor);
    else
      th->clearStyle();
  }

  return TCL_OK;
}

int
App::
fgColorProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if      (args[0] == "rgb") {
    if (args.size() < 4) return TCL_ERROR;

    int r = std::stoi(args[1]);
    int g = std::stoi(args[2]);
    int b = std::stoi(args[3]);

    COSRead::write(STDOUT_FILENO, CEscape::SGR_fg(r, g, b));
  }
  else if (args[0] == "palette") {
    if (args.size() < 2) return TCL_ERROR;

    int n = std::stoi(args[1]);

    COSRead::write(STDOUT_FILENO, CEscape::SGR_fg(n));
  }
  else if (args[0] == "bright") {
    if (args.size() != 2) return TCL_ERROR;

    int icolor = std::stoi(args[1]);

    if (icolor >= 0 && icolor <= 9)
      COSRead::write(STDOUT_FILENO, CEscape::SGR(100 + icolor));
  }
  else if (args[0] == "colorset") {
    if (args.size() != 3) return TCL_ERROR;

    auto name = args[1];
    auto ind  = std::stoi(args[2]);

    int r, g, b;

    if (colorSetColor(name, ind, r, g, b))
      COSRead::write(STDOUT_FILENO, CEscape::SGR_fg(r, g, b));
  }
  else {
    if (args.size() != 1) return TCL_ERROR;

    int icolor = std::stoi(args[0]);

    th->setColor(icolor);

    if (icolor >= 0 && icolor <= 9)
      th->fgIColor(icolor);
    else
      th->clearStyle();
  }

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
  if (args.size() < 2) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  std::string text;

  if (args.size() > 2)
    text = args[2];

  auto widgetName = "label." + std::to_string(th->numWidgets() + 1);

  auto *label = new Label(th, widgetName, row, col, text);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::labelWidgetProc), static_cast<Tcl::ObjCmdData>(label));

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
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if (args[1] == "text")
      label->setText(args[2]);
    else
      return TCL_ERROR;
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
listProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 3) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  Tcl::StringList strs;

  if (! th->tcl()->splitList(args[2], strs))
    return TCL_ERROR;

  auto widgetName = "tlist." + std::to_string(th->numWidgets() + 1);

  auto *list = new List(th, widgetName, row, col, strs);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::listWidgetProc), static_cast<Tcl::ObjCmdData>(list));

  th->addWidget(list);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
listWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *list = static_cast<List *>(clientData);
  assert(list);

  auto args = list->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if      (args[1] == "currentInd")
      list->app()->tcl()->setResult(list->currentInd());
    else if (args[1] == "currentText")
      list->app()->tcl()->setResult(list->currentText());
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if      (args[1] == "currentInd")
      list->setCurrentInd(std::stoi(args[2]));
    else if (args[1] == "width")
      list->setWidth(std::stoi(args[2]));
    else if (args[1] == "height")
      list->setHeight(std::stoi(args[2]));
    else
      return TCL_ERROR;
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
tableProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto errMsg = [&](const std::string &msg) {
    std::cerr << msg << "\n";
    return TCL_ERROR;
  };

  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  int  na   = int(args.size());

  int         row    = 0;
  int         col    = 0;
  int         width  = -1;
  int         height = -1;
  std::string data;
  bool        rows   = false;
  bool        header = false;

  StringList args1;

  for (int i = 0; i < na; ++i) {
    if (args[size_t(i)][0] == '-') {
      auto arg1 = args[size_t(i)].substr(1);

      if      (arg1 == "rows") {
        ++i;

        if (i < na) {
          data = args[size_t(i)];
          rows = true;
        }
        else
          return errMsg("Missing value for -rows");
      }
      else if (arg1 == "cols" || arg1 == "columns") {
        ++i;

        if (i < na) {
          data = args[size_t(i)];
          rows = false;
        }
        else
          return errMsg("Missing value for -rows");
      }
      else if (arg1 == "row") {
        ++i;

        if (i < na)
          row = std::stoi(args[size_t(i)]);
        else
          return errMsg("Missing value for -row");
      }
      else if (arg1 == "col") {
        ++i;

        if (i < na)
          col = std::stoi(args[size_t(i)]);
        else
          return errMsg("Missing value for -col");
      }
      else if (arg1 == "width") {
        ++i;

        if (i < na)
          width = std::stoi(args[size_t(i)]);
        else
          return errMsg("Missing value for -width");
      }
      else if (arg1 == "height") {
        ++i;

        if (i < na)
          height = std::stoi(args[size_t(i)]);
        else
          return errMsg("Missing value for -height");
      }
      else if (arg1 == "header") {
        header = true;
      }
      else
        return errMsg("Invalid arg '" + args[size_t(i)] + "'");
    }
    else
      args1.push_back(args[size_t(i)]);
  }

  if (! args1.empty())
    return errMsg("Invalid extra arguments");

  // get data
  Tcl::StringList strs;

  if (! th->tcl()->splitList(data, strs))
    return TCL_ERROR;

  using ColStrs = std::vector<StringList>;

  ColStrs colStrs;
  int     nr { 0 }, nc { 0 };

  if (! rows) {
    nc = int(strs.size());
    nr = 0;

    for (int c = 0; c < nc; ++c) {
      Tcl::StringList cstrs;

      if (! th->tcl()->splitList(strs[size_t(c)], cstrs))
        return TCL_ERROR;

      nr = std::max(nr, int(cstrs.size()));

      colStrs.push_back(cstrs);
    }

    for (int c = 0; c < nc; ++c) {
      while (int(colStrs[size_t(c)].size()) < nr)
        colStrs[size_t(c)].push_back("");
    }
  }
  else {
    using RowStrs = std::vector<StringList>;

    RowStrs rowStrs;

    nr = int(strs.size());
    nc = 0;

    for (int r = 0; r < nr; ++r) {
      Tcl::StringList rstrs;

      if (! th->tcl()->splitList(strs[size_t(r)], rstrs))
        return TCL_ERROR;

      nc = std::max(nc, int(rstrs.size()));

      rowStrs.push_back(rstrs);
    }

    for (int r = 0; r < nr; ++r) {
      while (int(rowStrs[size_t(r)].size()) < nc)
        rowStrs[size_t(r)].push_back("");
    }

    for (int c = 0; c < nc; ++c) {
      StringList cstrs;

      for (int r = 0; r < nr; ++r)
        cstrs.push_back(rowStrs[size_t(r)][size_t(c)]);

      colStrs.push_back(cstrs);
    }
  }

  auto widgetName = "ttable." + std::to_string(th->numWidgets() + 1);

  auto *table = new Table(th, widgetName, row, col, nr, nc, colStrs);

  table->setHeader(header);

  if (width > 0)
    table->setWidth(width);

  if (height > 0)
    table->setHeight(height);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::tableWidgetProc), static_cast<Tcl::ObjCmdData>(table));

  th->addWidget(table);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
tableWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *table = static_cast<Table *>(clientData);
  assert(table);

  auto args = table->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if (args[1] == "currentRow")
      table->app()->tcl()->setResult(table->currentRow());
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if      (args[1] == "currentRow")
      table->setCurrentRow(std::stoi(args[2]));
    else if (args[1] == "item") {
      if (args.size() < 5) return TCL_ERROR;

      int row = std::stoi(args[2]);
      int col = std::stoi(args[3]);

      table->setItem(row, col, args[4]);
    }
    else if (args[1] == "width")
      table->setWidth(std::stoi(args[2]));
    else if (args[1] == "height")
      table->setHeight(std::stoi(args[2]));
    else
      return TCL_ERROR;
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

  auto widgetName = "tcheck." + std::to_string(th->numWidgets() + 1);

  auto *check = new Check(th, widgetName, row, col);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::checkWidgetProc), static_cast<Tcl::ObjCmdData>(check));

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
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if (args[1] == "checked")
      check->setChecked(std::stoi(args[2]));
    else
      return TCL_ERROR;
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
inputProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 2) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);

  auto widgetName = "tinput." + std::to_string(th->numWidgets() + 1);

  auto *input = new Input(th, widgetName, row, col);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::inputWidgetProc), static_cast<Tcl::ObjCmdData>(input));

  th->addWidget(input);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
inputWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *input = static_cast<Input *>(clientData);
  assert(input);

  auto args = input->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if      (args[1] == "text")
      input->app()->tcl()->setResult(input->text());
    else if (args[1] == "width")
      input->app()->tcl()->setResult(input->width());
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if      (args[1] == "text")
      input->setText(args[2]);
    else if (args[1] == "width")
      input->setWidth(std::stoi(args[2]));
    else
      return TCL_ERROR;
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
boxProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 4) return TCL_ERROR;

  int row = std::stoi(args[0]);
  int col = std::stoi(args[1]);
  int w   = std::stoi(args[2]);
  int h   = std::stoi(args[3]);

  auto widgetName = "tbox." + std::to_string(th->numWidgets() + 1);

  auto *box = new Box(th, widgetName, row, col, w, h);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::boxWidgetProc), static_cast<Tcl::ObjCmdData>(box));

  th->addWidget(box);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
boxWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *box = static_cast<Box *>(clientData);
  assert(box);

  auto args = box->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if      (args[1] == "width")
      box->app()->tcl()->setResult(box->width());
    else if (args[1] == "height")
      box->app()->tcl()->setResult(box->height());
    else if (args[1] == "fill")
      box->app()->tcl()->setResult(box->fill());
    else if (args[1] == "fill_rgb") {
      CRGBA rgba;

      CRGBName::toRGBA(box->fill(), rgba);

      std::vector<int> res { int(rgba.getRedI()), int(rgba.getGreenI()), int(rgba.getBlueI()) };

      box->app()->tcl()->setResult(res);
    }
    else if (args[1] == "fill_hsv") {
      CRGBA rgba;

      CRGBName::toRGBA(box->fill(), rgba);

      auto hsv = CRGBUtil::RGBtoHSV(rgba.getRGB());

      std::vector<int> res { int(hsv.getHueI()), int(hsv.getSaturationI()), int(hsv.getValueI()) };

      box->app()->tcl()->setResult(res);
    }
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if      (args[1] == "width")
      box->setWidth(std::stoi(args[2]));
    else if (args[1] == "height")
      box->setHeight(std::stoi(args[2]));
    else if (args[1] == "fill")
      box->setFill(args[2]);
    else
      return TCL_ERROR;
  }
  else {
    return TCL_ERROR;
  }

  return TCL_OK;
}

//---

int
App::
imageProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() != 5) return TCL_ERROR;

  int  row  = std::stoi(args[0]);
  int  col  = std::stoi(args[1]);
  int  w    = std::stoi(args[2]);
  int  h    = std::stoi(args[3]);
  auto file = args[4];

  auto widgetName = "tbox." + std::to_string(th->numWidgets() + 1);

  auto *image = new Image(th, widgetName, row, col, w, h, file);

  th->tcl()->createObjCommand(widgetName,
    reinterpret_cast<Tcl::ObjCmdProc>(&App::imageWidgetProc), static_cast<Tcl::ObjCmdData>(image));

  th->addWidget(image);

  th->redraw();

  th->tcl()->setResult(widgetName);

  return TCL_OK;
}

int
App::
imageWidgetProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *image = static_cast<Image *>(clientData);
  assert(image);

  auto args = image->app()->getArgs(objc, objv);
  if (args.size() < 1) return TCL_ERROR;

  if       (args[0] == "get") {
    if (args.size() < 2) return TCL_ERROR;

    if      (args[1] == "width")
      image->app()->tcl()->setResult(image->width());
    else if (args[1] == "height")
      image->app()->tcl()->setResult(image->height());
    else if (args[1] == "file")
      image->app()->tcl()->setResult(image->file());
    else
      return TCL_ERROR;
  }
  else if (args[0] == "set") {
    if (args.size() < 3) return TCL_ERROR;

    if      (args[1] == "width")
      image->setWidth(std::stoi(args[2]));
    else if (args[1] == "height")
      image->setHeight(std::stoi(args[2]));
    else if (args[1] == "file")
      image->setFile(args[2]);
    else
      return TCL_ERROR;
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

//---

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
    return TCL_ERROR;

  return TCL_OK;
}

//---

int
App::
ttyStateProc(void *clientData, Tcl_Interp *, int objc, const Tcl_Obj **objv)
{
  auto *th = static_cast<App *>(clientData);
  assert(th);

  auto args = th->getArgs(objc, objv);
  if (args.size() < 2) return TCL_ERROR;

  std::string op   = CStrUtil::toLower(args[0]);
  std::string name = CStrUtil::toLower(args[1]);

  if      (op == "get") {
  }
  else if (op == "set") {
    if (args.size() < 3) return TCL_ERROR;

    std::string value = CStrUtil::toLower(args[2]);

    if      (name == "inverse_video")
      COSRead::write(STDOUT_FILENO, CEscape::DECSCNM(value == "1")); // DECSET/DECRST 5
    else if (name == "show_cursor") {
      if (value == "1")
        COSRead::write(STDOUT_FILENO, CEscape::DECSET(25));
      else
        COSRead::write(STDOUT_FILENO, CEscape::DECRST(25));
    }
    else if (name == "blink_cursor") {
      if (value == "1")
        COSRead::write(STDOUT_FILENO, CEscape::DECSET(12));
      else
        COSRead::write(STDOUT_FILENO, CEscape::DECRST(12));
    }
    else if (name == "scrollbar") {
      if (value == "1")
        COSRead::write(STDOUT_FILENO, CEscape::DECSET(30));
      else
        COSRead::write(STDOUT_FILENO, CEscape::DECRST(30));
    }
  }

  return TCL_OK;
}

//---

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

    th->updateWindowCharSize();
  }
  else {
    th->setRaw(false);
  }

  return TCL_OK;
}

//---

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

//------

App::StringList
App::
getArgs(int objc, const Tcl_Obj **objv) const
{
  App::StringList args;

  for (int i = 1; i < objc; ++i)
    args.push_back(CTclUtil::stringFromObj(const_cast<Tcl_Obj *>(objv[size_t(i)])));

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

    uint len = uint(buffer.size());

    if (len == 0) continue;

    processString(buffer);

    if (isDone()) break;

    redraw();
  }
}

void
App::
updateWindowCharSize()
{
  if (CEscape::getWindowCharSize(&screenRows_, &screenCols_)) {
    std::vector<int> ivals; ivals.push_back(screenRows_); ivals.push_back(screenCols_);

    tcl_->createVar("window_char_size", ivals);
  }
}

void
App::
updateWindowPixelSize()
{
  if (CEscape::getWindowPixelSize(&screenWidth_, &screenHeight_)) {
    std::vector<int> ivals; ivals.push_back(screenWidth_); ivals.push_back(screenHeight_);

    tcl_->createVar("window_pixel_size", ivals);
  }
}

void
App::
updateWindowPos()
{
  int row, col;

  if (CEscape::getWindowPos(&row, &col)) {
    std::vector<int> ivals; ivals.push_back(row); ivals.push_back(col);

    tcl_->createVar("window_pos", ivals);
  }
}

void
App::
redraw()
{
  if (in_redraw_)
    return;

  in_redraw_ = true;

  COSRead::write(STDOUT_FILENO, CEscape::ED(2)); // all

  std::string res;

  tcl_->eval("redrawProc", res, /*showError*/true);

  for (const auto &w : widgets_)
    w->draw();

  in_redraw_ = false;
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
  uint len = uint(str.size());

  //---

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
      //CMouseEvent event(pos, (CMouseButton) (button + 1));

      //mousePress(event);
      MouseData data;
      data.button = button;
      mousePress(data);

      pressButton = button;
    }
    else {
      //CMouseEvent event(pos, (CMouseButton) (pressButton + 1));

      //mouseRelease(event);
      MouseData data;
      data.button = pressButton;
      mouseRelease(data);
    }

    return;
  }

  //---

  // skip status report
  if (len > 8 && str[0] == '' && str[1] == '[' && str[len - 1] == 't')
    return;

  //---

  for (uint pos = 0; pos < len; ++pos) {
    if (processStringChar(static_cast<unsigned char>(str[size_t(pos)])))
      i = pos + 1;

    if (done_) return;
  }

  if (i < len) {
    for (uint pos = i; pos < len; ++pos) {
      processChar(static_cast<unsigned char>(str[size_t(pos)]));

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
      escapeString_ = std::string(reinterpret_cast<char *>(&c), 1);
      return false;
    }

    processChar(c);

    return true;
  }
  else {
    escapeString_ += char(c);

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
    case 0     : { data.type = CKEY_TYPE_NUL        ; break; }
    case ''  : { data.type = CKEY_TYPE_SOH        ; data.text = "line_begin"    ; break; }
    case ''  : { data.type = CKEY_TYPE_STX        ; data.text = "line_left"     ; break; }
    case ''  : { data.type = CKEY_TYPE_ETX        ; data.text = "cancel"        ; break; }
    case ''  : { data.type = CKEY_TYPE_EOT        ; data.text = "line_del"      ; break; }
    case ''  : { data.type = CKEY_TYPE_ENQ        ; data.text = "line_end"      ; break; }
    case ''  : { data.type = CKEY_TYPE_ACK        ; data.text = "line_right"    ; break; }
    case ''  : { data.type = CKEY_TYPE_BEL        ; data.text = "bell"          ; break; }
    case ''  : { data.type = CKEY_TYPE_BackSpace  ; data.text = "backspace"     ; break; }
    case '\011': { data.type = CKEY_TYPE_TAB        ; data.text = "tab"           ; break; }
    case '\012': { data.type = CKEY_TYPE_LineFeed   ; data.text = "lf"            ; break; }
    case ''  : { data.type = CKEY_TYPE_Clear      ; data.text = "line_clear_end"; break; }
    case ''  : { data.type = CKEY_TYPE_FF         ; data.text = "ff"            ; break; }
    case '\015': { data.type = CKEY_TYPE_Return     ; data.text = "return"        ; break; }
    case ''  : { data.type = CKEY_TYPE_SO         ; data.text = "next"          ; break; }
    case ''  : { data.type = CKEY_TYPE_SI         ; break; }
    case ''  : { data.type = CKEY_TYPE_DLE        ; data.text = "previous"      ; break; }
    case '\021': { data.type = CKEY_TYPE_DC1        ; data.text = "start"         ; break; }
    case ''  : { data.type = CKEY_TYPE_DC2        ; data.text = "search"        ; break; }
    case '\023': { data.type = CKEY_TYPE_Pause      ; data.text = "stop"          ; break; }
    case ''  : { data.type = CKEY_TYPE_Scroll_Lock; data.text = "transpose"     ; break; }
    case ''  : { data.type = CKEY_TYPE_Sys_Req    ; data.text = "line_clear"    ; break; }
    case ''  : { data.type = CKEY_TYPE_SYN        ; data.text = "paste"         ; break; }
    case ''  : { data.type = CKEY_TYPE_ETB        ; data.text = "delete_word"   ; break; }
    case ''  : { data.type = CKEY_TYPE_CAN        ; data.text = "cursor_prev"   ; break; }
    case ''  : { data.type = CKEY_TYPE_EM         ; data.text = "redo"          ; break; }
    case ''  : { data.type = CKEY_TYPE_SUB        ; data.text = "undo"          ; break; }
    case '\033': { data.type = CKEY_TYPE_Escape     ; data.text = "escape"        ; break; } // [
    case '\034': { data.type = CKEY_TYPE_FS         ; data.text = "kill"          ; break; } /* \ */
    case '\035': { data.type = CKEY_TYPE_GS         ;                               break; } // ]
    case '\036': { data.type = CKEY_TYPE_RS         ;                               break; } // ^
    case '\037': { data.type = CKEY_TYPE_US         ;                               break; } // _

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

    case 0x7f  : { data.type = CKEY_TYPE_DEL         ; data.text = "del";  break; }
    default    : {                                     break; }
  }

  if (c >= ' ' && c <= '}')
    data.text = std::string(reinterpret_cast<char *>(&c), 1);

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
  else if (data.text == "escape" || data.text == "cancel") {
    setDone(true);

    return;
  }

  for (auto *w : focusWidgets_)
    w->keyPress(data);

  std::string res;

  tcl_->eval("keyPressProc " + std::to_string(int(data.type)) + " {" + data.text + "}",
              res, /*showError*/true);
}

void
App::
mousePress(const MouseData &)
{
  std::string res;

  tcl_->eval("mousePressProc", res, /*showError*/true);
}

void
App::
mouseRelease(const MouseData &)
{
  std::string res;

  tcl_->eval("mouseReleaseProc", res, /*showError*/true);
}

//---

void
App::
fgIColor(int i)
{
  COSRead::write(STDOUT_FILENO, CEscape::SGR(30 + i));
}

void
App::
bgIColor(int i)
{
  COSRead::write(STDOUT_FILENO, CEscape::SGR(40 + i));
}

void
App::
drawBox(int r1, int c1, int r2, int c2) const
{
  // corners
  drawString(r1, c1, "\u250c" /*'+'*/);
  drawString(r1, c2, "\u2510" /*'+'*/);
  drawString(r2, c1, "\u2514" /*'+'*/);
  drawString(r2, c2, "\u2518" /*'+'*/);

  // sides
  for (int r = r1 + 1; r <= r2 - 1; ++r) {
    drawString(r, c1, "\u2502" /*'|'*/);
    drawString(r, c2, "\u2502" /*'|'*/);
  }

  for (int c = c1 + 1; c <= c2 - 1; ++c) {
    drawString(r1, c, "\u2500" /*'-'*/);
    drawString(r2, c, "\u2500" /*'-'*/);
  }
}

void
App::
fillBox(int r1, int c1, int r2, int c2) const
{
  for (int r = r1; r <= r2; ++r) {
    for (int c = c1; c <= c2; ++c) {
      drawString(r, c, "\u2588");
    }
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
drawString(int row, int col, const std::string &s) const
{
  moveTo(row, col);

  COSRead::write(STDOUT_FILENO, s);
}

void
App::
drawImage(int r1, int c1, int r2, int c2, const CImagePtr &image) const
{
  int ih = image->getHeight();
  int ix = 0;

  for (int c = c1; c <= c2; ++c, ++ix) {
    int iy = 0;

    for (int r = r1; r <= r2; ++r, iy += 2) {
      uint ir1, ig1, ib1, ia1;
      image->getRGBAPixelI(ix, iy, &ir1, &ig1, &ib1, &ia1);

      uint ir2 = 0, ig2 = 0, ib2 = 0, ia2 = 255;
      if (iy < ih)
        image->getRGBAPixelI(ix, iy + 1, &ir2, &ig2, &ib2, &ia2);

      if (ia1 > 0)
        COSRead::write(STDOUT_FILENO, CEscape::SGR_bg(ir1, ig1, ib1));
      else
        COSRead::write(STDOUT_FILENO, CEscape::SGR_bg(0, 0, 0));

      if (ia2 > 0)
        COSRead::write(STDOUT_FILENO, CEscape::SGR_fg(ir2, ig2, ib2));
      else
        COSRead::write(STDOUT_FILENO, CEscape::SGR_fg(0, 0, 0));

      drawString(r, c, "\u2584" /*▄*/);
    }
  }
}

void
App::
moveTo(int row, int col) const
{
  COSRead::write(STDOUT_FILENO, CEscape::CUP(row, col));
}

void
App::
writeText(const char *text) const
{
  COSRead::write(STDOUT_FILENO, text);
}

void
App::
clearStyle()
{
  COSRead::write(STDOUT_FILENO, CEscape::SGR(0));
}

void
App::
outputError(const std::string &msg)
{
  moveTo(1, 1);

  writeText(msg.c_str());

  if (errorMsg_ != "")
    errorMsg_ += "\n";

  errorMsg_ += msg;
}

void
App::
handleRead(const std::string &name)
{
  if      (name == "window_pos" || name == "::window_pos")
    updateWindowPos();
  else if (name == "window_char_size" || name == "::window_char_size")
    updateWindowCharSize();
  else if (name == "window_pixel_size" || name == "::window_pixel_size")
    updateWindowPixelSize();
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

      clearStyle();

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
  app_->drawString(row_, col_, text_);
}

//---

List::
List(App *app, const std::string &name, int row, int col, const StringList &strs) :
 Widget(app, name), row_(row), col_(col), strs_(strs)
{
  width_ = 0;

  for (const auto &str : strs_)
    width_ = std::max(width_, int(str.size()));

  height_ = int(strs_.size());

  n_ = int(strs_.size());
}

void
List::
setCurrentInd(int i)
{
  if (i < 0 || i >= int(strs_.size()))
    return;

  current_ = i;

  app_->redraw();
}

std::string
List::
currentText() const
{
  if (current_ < 0 || current_ >= int(strs_.size()))
    return "";

  return strs_[size_t(current_)];
}

void
List::
draw() const
{
  int focusColor = 5;

  if (hasFocus())
    app_->fgIColor(focusColor);
  else
    app_->clearStyle();

  app_->drawBox(row_, col_, row_ + height_ + 1, col_ + width_ + 5);

  app_->clearStyle();

  int cursorColor = 3;

  int y = 0;

  for (int i = 0; i < n_; ++i) {
    if (i < -yOffset_)
      continue;

    if (y >= height_)
      break;

    const auto &str = strs_[size_t(y - yOffset_)];

    app_->moveTo(y + row_ + 1, col_ + 1);

    if (i == current_) {
      if (hasFocus())
        app_->fgIColor(cursorColor);
      else
        app_->clearStyle();

      COSRead::write(STDOUT_FILENO, "->");

      app_->clearStyle();
    }
    else
      COSRead::write(STDOUT_FILENO, "  ");

    app_->drawString(y + row_ + 1, col_ + 4, str);

    ++y;
  }

  app_->moveTo(y + row_ + 3, 0);
}

void
List::
keyPress(const KeyData &data)
{
  if (! hasFocus())
    return;

  std::string res;

  if      (data.text == "home") {
    if (current_ <= 0) return;

    current_ = 0;

    app_->redraw();

    app_->tcl()->eval("tlistIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "end") {
    if (current_ >= int(n_ - 1)) return;

    current_ = n_ - 1;

    app_->redraw();

    app_->tcl()->eval("tlistIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "down") {
    if (current_ >= int(n_ - 1)) return;

    ++current_;

    if (current_ + yOffset_ + 1 > height_) {
      if (yOffset_ > height_ - n_)
        --yOffset_;
    }

    app_->redraw();

    app_->tcl()->eval("tlistIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "up") {
    if (current_ <= 0) return;

    --current_;

    if (current_ + yOffset_ - 1 < 0) {
      if (yOffset_ < 0)
        ++yOffset_;
    }

    app_->redraw();

    app_->tcl()->eval("tlistIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "return") {
    app_->tcl()->eval("tlistExecProc {" + name() + "}", res, /*showError*/true);
  }
}

//---

Table::
Table(App *app, const std::string &name, int row, int col, int nr, int nc,
      const ColStringList &colStrs) :
 Widget(app, name), row_(row), col_(col), nr_(nr), nc_(nc), colStrs_(colStrs)
{
  for (int c = 0; c < nc; ++c) {
    colWidth_[c] = 0;

    for (const auto &str : colStrs_[size_t(c)])
      colWidth_[c] = std::max(colWidth_[c], int(str.size()));
  }

  width_ = 0;

  for (int c = 0; c < nc; ++c)
    width_ += colWidth_[c] + 3;

  height_ = nr_;
}

void
Table::
setCurrentRow(int r)
{
  int headerOffset = (isHeader() ? 1 : 0);

  if (r < 0 || r >= nr_ - headerOffset)
    return;

  currentRow_ = r;

  app_->redraw();
}

void
Table::
setItem(int r, int c, const std::string &text)
{
  if (r < 0 || r >= nr_ || c < 0 || c >= nc_)
    return;

  colStrs_[size_t(c)][size_t(r)] = text;

  app_->redraw();
}

void
Table::
draw() const
{
  int screenRows = app_->screenRows();
  int screenCols = app_->screenCols();

  //---

  auto *th = const_cast<Table *>(this);

  //---

  int focusColor = 5;

  if (hasFocus())
    app_->fgIColor(focusColor);
  else
    app_->clearStyle();

  int height = height_;

  int row2 = row_ + height + 1;
  int col2 = col_ + width_ + 2;

  if (row2 >= screenRows) {
    row2   = screenRows - 1;
    height = screenRows - row_ - 2;
  }

  if (col2 >= screenCols)
    col2 = screenCols - 1;

  app_->drawBox(row_, col_, row2, col2);

  app_->clearStyle();

  int cursorColor = 3;

  int headerOffset = (isHeader() ? 1 : 0);

  int y = 0;

  for (int r = 0; r < nr_; ++r) {
    bool headerRow = (isHeader() && r == 0);

    if (! headerRow && r < -yOffset_)
      continue;

    if (y >= height)
      break;

    int x = col_ + 1;

    for (int c = 0; c < nc_; ++c) {
      app_->moveTo(y + row_ + 1, x);

      if (c == 0) {
        if (x + 2 > screenCols)
          break;

        if (r == currentRow_ + headerOffset) {
          if (hasFocus())
            app_->fgIColor(cursorColor);
          else
            app_->clearStyle();

          COSRead::write(STDOUT_FILENO, "->");

          app_->clearStyle();
        }
        else
          COSRead::write(STDOUT_FILENO, "  ");
      }
      else {
        if (x + 1 > screenCols)
          break;

        if (hasFocus())
          app_->fgIColor(focusColor);

        COSRead::write(STDOUT_FILENO, " \u2502" /*'|'*/);
      }

      x += 2;

      const auto &str = (headerRow ?
        colStrs_[size_t(c)][0] : colStrs_[size_t(c)][size_t(y - yOffset_)]);

      if (x + int(str.size()) + 1 > screenCols)
        break;

      app_->clearStyle();

      if (headerRow) {
        app_->bgIColor(2);

        COSRead::write(STDOUT_FILENO, CEscape::SGR(1));
      }

      app_->drawString(y + row_ + 1, x + 1, str);

      if (headerRow)
        app_->clearStyle();

      x += th->colWidth_[c] + 1;
    }

    ++y;
  }

  app_->moveTo(y + row_ + 3, 0);
}

void
Table::
keyPress(const KeyData &data)
{
  if (! hasFocus())
    return;

  std::string res;

  if      (data.text == "home") {
    if (currentRow_ <= 0) return;

    currentRow_ = 0;

    app_->redraw();

    app_->tcl()->eval("ttableIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "end") {
    int headerOffset = (isHeader() ? 1 : 0);

    if (currentRow_ >= int(nr_ - headerOffset - 1)) return;

    currentRow_ = nr_ - headerOffset - 1;

    app_->redraw();

    app_->tcl()->eval("ttableIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "down") {
    int headerOffset = (isHeader() ? 1 : 0);

    if (currentRow_ >= int(nr_ - headerOffset - 1)) return;

    ++currentRow_;

    int screenRows = app_->screenRows();

    int height = height_;
    int row2   = row_ + height + 1;

    if (row2 >= screenRows)
      height = screenRows - row_ - 2;

    if (currentRow_ + yOffset_ + 1 > height - headerOffset) {
      if (yOffset_ > height - nr_ - headerOffset)
        --yOffset_;
    }

    app_->redraw();

    app_->tcl()->eval("ttableIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "up") {
    if (currentRow_ <= 0) return;

    --currentRow_;

    if (currentRow_ + yOffset_ - 1 < 0) {
      if (yOffset_ < 0)
        ++yOffset_;
    }

    app_->redraw();

    app_->tcl()->eval("ttableIndexChangedProc {" + name() + "}", res, /*showError*/true);
  }
  else if (data.text == "return") {
    app_->tcl()->eval("ttableExecProc {" + name() + "}", res, /*showError*/true);
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
    app_->fgIColor(focusColor);
  else
    app_->clearStyle();

  app_->drawBox(row_, col_, row_ + 2, col_ + 2);

  app_->clearStyle();

  if (checked_)
    app_->drawString(row_ + 1, col_ + 1, "\u2713" /*"x"*/);
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

    app_->tcl()->eval("tcheckStateChangedProc {" + name() + "}", res, /*showError*/true);

    app_->redraw();
  }
}

//---

Input::
Input(App *app, const std::string &name, int row, int col) :
 Widget(app, name), row_(row), col_(col)
{
}

void
Input::
setWidth(int i)
{
  width_ = i;

  app_->redraw();
}

void
Input::
setText(const std::string &str)
{
  text_ = str;

  app_->redraw();
}

void
Input::
setCursorPos(int i)
{
  cursorPos_ = i;

  app_->redraw();
}

void
Input::
draw() const
{
  int focusColor = 5;

  if (hasFocus())
    app_->fgIColor(focusColor);
  else
    app_->clearStyle();

  app_->drawBox(row_, col_, row_ + 2, col_ + width_ + 2);

  auto *th = const_cast<Input *>(this);

  if      (cursorPos_ < 0                  ) th->cursorPos_ = 0;
  else if (cursorPos_ > int(text_.length())) th->cursorPos_ = int(text_.length());

  int dx = 0;

  if (cursorPos_ > width_)
    dx = cursorPos_ - width_;

  for (int i = 0; i <= int(text_.size()); ++i) {
    if (i < dx)
      continue;

    if (i - dx > width_)
      break;

    app_->moveTo(row_ + 1, col_ + 1 + i - dx);

    if (i == cursorPos_)
      app_->bgIColor(3);
    else
      app_->clearStyle();

    if (i < int(text_.length()))
      COSRead::write(STDOUT_FILENO, text_[size_t(i)]);
    else
      COSRead::write(STDOUT_FILENO, " ");
  }

  app_->clearStyle();
}

void
Input::
keyPress(const KeyData &data)
{
  if (! hasFocus())
    return;

  if      (data.text != "" && data.text.length() == 1) {
    auto lhs = text_.substr(0, size_t(cursorPos_));
    auto rhs = (cursorPos_ < int(text_.length()) ? text_.substr(size_t(cursorPos_)) : "");

    text_ = lhs + data.text + rhs;

    ++cursorPos_;

    app_->redraw();
  }
  else if (data.text == "backspace") {
    if (cursorPos_ > 0) {
      auto lhs = text_.substr(0, size_t(cursorPos_ - 1));
      auto rhs = (cursorPos_ < int(text_.length()) ? text_.substr(size_t(cursorPos_)) : "");

      text_ = lhs + rhs;

      --cursorPos_;

      app_->redraw();
    }
  }
  else if (data.text == "del" || data.text == "line_del") {
    if (cursorPos_ < int(text_.length())) {
      auto lhs = text_.substr(0, size_t(cursorPos_));
      auto rhs = (cursorPos_ + 1 < int(text_.length()) ? text_.substr(size_t(cursorPos_ + 1)) : "");

      text_ = lhs + rhs;

      app_->redraw();
    }
  }
  else if (data.text == "line_clear") {
    if (cursorPos_ < int(text_.length())) {
      auto lhs = text_.substr(0, size_t(cursorPos_));

      text_ = lhs;

      app_->redraw();
    }
  }
  else if (data.text == "line_begin") {
    if (cursorPos_ > 0) {
      cursorPos_ = 0;

      app_->redraw();
    }
  }
  else if (data.text == "line_end") {
    if (cursorPos_ < int(text_.length())) {
      cursorPos_ = int(text_.length());

      app_->redraw();
    }
  }
  else if (data.text == "left" || data.text == "line_left") {
    if (cursorPos_ > 0) {
      --cursorPos_;

      app_->redraw();
    }
  }
  else if (data.text == "right" || data.text == "line_right") {
    if (cursorPos_ < int(text_.length())) {
      ++cursorPos_;

      app_->redraw();
    }
  }
  else if (data.text == "return") {
    std::string res;

    app_->tcl()->eval("tinputExecProc {" + name() + "}", res, /*showError*/true);
  }
}

//---

Box::
Box(App *app, const std::string &name, int row, int col, int width, int height) :
 Widget(app, name), row_(row), col_(col), width_(width), height_(height)
{
}

void
Box::
setWidth(int i)
{
  width_ = i;

  app_->redraw();
}

void
Box::
setHeight(int i)
{
  height_ = i;

  app_->redraw();
}

void
Box::
setFill(const std::string &s)
{
  fill_ = s;

  app_->redraw();
}

void
Box::
draw() const
{
  app_->drawBox(row_, col_, row_ + height_ - 1, col_ + width_ - 1);

  if (fill_ != "") {
    CRGBA rgba;

    CRGBName::toRGBA(fill_, rgba);

    COSRead::write(STDOUT_FILENO,
      CEscape::SGR_fg(int(rgba.getRedI()), int(rgba.getGreenI()), int(rgba.getBlueI())));

    app_->fillBox(row_, col_, row_ + height_ - 1, col_ + width_ - 1);
  }

  app_->clearStyle();
}

//---

Image::
Image(App *app, const std::string &name, int row, int col, int width, int height,
      const std::string &file) :
 Widget(app, name), row_(row), col_(col), width_(width), height_(height), file_(file)
{
  updateImage();
}

Image::
~Image()
{
}

void
Image::
setWidth(int i)
{
  width_ = i;

  updateImage();

  app_->redraw();
}

void
Image::
setHeight(int i)
{
  height_ = i;

  updateImage();

  app_->redraw();
}

void
Image::
setFile(const std::string &s)
{
  file_ = s;

  updateImage();

  app_->redraw();
}

void
Image::
draw() const
{
  app_->drawImage(row_, col_, row_ + height_ - 1, col_ + width_ - 1, image_);
}

void
Image::
updateImage()
{
  CImageFileSrc src(file_);

  image_ = CImageMgrInst->createImage(src);

  image_ = image_->resize(width_, height_*2);
}

//------

}
