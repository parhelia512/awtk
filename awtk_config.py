import os
import sys
import glob
import os.path

sys.path.insert(0, os.path.join(os.path.abspath(os.path.dirname(__file__)), 'scripts'))
import compile_config
compile_helper = compile_config.get_curr_config_for_awtk()

from awtk_config_common import OS_NAME, TARGET_ARCH, TOOLS_PREFIX, TK_SRC, TK_BIN_DIR, TK_LIB_DIR, TK_3RD_ROOT, TK_TOOLS_ROOT, OS_DEBUG, TK_DEMO_ROOT, GTEST_ROOT, TKC_STATIC_LIBS, TOOLS_NAME, NANOVG_BACKEND, NATIVE_WINDOW, TK_ROOT
from awtk_config_common import joinPath, toWholeArchive, genIdlAndDefEx, setEnvSpawn, genDllLinkFlags, copySharedLib, cleanSharedLib, scons_db_check_and_remove, is_raspberrypi
from awtk_config_common import OS_FLAGS, OS_LIBS, OS_LIBPATH, OS_CPPPATH, OS_LINKFLAGS, OS_SUBSYSTEM_CONSOLE, OS_SUBSYSTEM_WINDOWS, OS_PROJECTS, COMMON_CFLAGS

WIN32_AWTK_RES = 'win32_res/awtk.res'
if TARGET_ARCH == 'x86':
    WIN32_AWTK_RES = 'win32_res/awtk_x86.res'

WIN32_AWTK_RES = os.path.join(compile_config.get_curr_app_root(), WIN32_AWTK_RES)
if not os.path.exists(WIN32_AWTK_RES):
    if TARGET_ARCH == 'x86':
        WIN32_AWTK_RES = os.path.join(TK_ROOT, 'win32_res/awtk_x86.res')
    else:
        WIN32_AWTK_RES = os.path.join(TK_ROOT, 'win32_res/awtk.res')

WIN32_AWTK_RES = compile_helper.get_value('WIN32_RES', WIN32_AWTK_RES)
if not os.path.isabs(WIN32_AWTK_RES) :
    WIN32_AWTK_RES = os.path.join(compile_config.get_curr_app_root(), WIN32_AWTK_RES)

AWTK_STATIC_LIBS = ['awtk_global', 'fscript_ext_widgets', 'extwidgets',
                    'widgets', 'base', 'gpinyin', 'fribidi', 'linebreak', 'svgtiny']

if os.getenv('SDL_VIDEODRIVER') is None:
  AWTK_STATIC_LIBS += ['nfd']

AWTK_STATIC_LIBS = AWTK_STATIC_LIBS+TKC_STATIC_LIBS

# INPUT_ENGINE='null'
# INPUT_ENGINE='spinyin'
# INPUT_ENGINE='t9'
# INPUT_ENGINE='t9ext'
INPUT_ENGINE = 'pinyin'

INPUT_ENGINE = compile_helper.get_value('INPUT_ENGINE', INPUT_ENGINE)

VGCANVAS = 'NANOVG'
VGCANVAS='NANOVG_PLUS'
# VGCANVAS='CAIRO'

VGCANVAS = compile_helper.get_value('VGCANVAS', VGCANVAS)

if OS_NAME == 'Windows':
    TK_ROOT = TK_ROOT.replace('\\', '\\\\')
NANOVG_BACKEND = 'GL3'

# NANOVG_BACKEND='GLES2'
# NANOVG_BACKEND='GLES3'
# NANOVG_BACKEND='AGG'
# NANOVG_BACKEND='AGGE'
# NANOVG_BACKEND='GL2'
if is_raspberrypi():
  NANOVG_BACKEND='GLES2'

NANOVG_BACKEND = compile_helper.get_value('NANOVG_BACKEND', NANOVG_BACKEND)
#NANOVG_BACKEND='BGFX'

FRAME_BUFFER_FORMAT = ''
if VGCANVAS == 'CAIRO':
    LCD = 'SDL_FB'
    FRAME_BUFFER_FORMAT = compile_helper.get_value('LCD_COLOR_FORMAT', 'bgr565')
    # FRAME_BUFFER_FORMAT='bgra8888'
else:
    if NANOVG_BACKEND == 'AGGE' or NANOVG_BACKEND == 'AGG':
        LCD = 'SDL_FB'
        FRAME_BUFFER_FORMAT = 'bgr565'
        # FRAME_BUFFER_FORMAT='bgra8888'
    else:
        LCD = 'SDL_GPU'
# LCD='SDL_FB_MONO'

if NANOVG_BACKEND == 'AGGE' or NANOVG_BACKEND == 'AGGE' :
    lcd_color_format = compile_helper.get_value('LCD_COLOR_FORMAT', '')
    if lcd_color_format != '' :
        if lcd_color_format== 'mono' :
            LCD = 'SDL_FB_MONO'
        else :
            LCD = 'SDL_FB'
            if lcd_color_format == 'bgr565' :
                FRAME_BUFFER_FORMAT = lcd_color_format
            else :
                FRAME_BUFFER_FORMAT = 'bgra8888'


NANOVG_BACKEND_LIBS = []
NANOVG_BACKEND_PROJS = []
NANOVG_BACKEND_CPPPATH = []

NATIVE_WINDOW = 'sdl'

COMMON_CCFLAGS = ' -DTK_ROOT=\"\\\"'+TK_ROOT+'\\\"\" '
#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITHOUT_WINDOW_ANIMATOR_CACHE=1 '
#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DENABLE_PERFORMANCE_PROFILE=1 '
#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DENABLE_MEM_LEAK_CHECK=1 '
#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_STATE_ACTIVATED=1 '

OPENGL_ANTIALIAS = compile_helper.get_value('OPENGL_ANTIALIAS', 'HW');
if OPENGL_ANTIALIAS == 'HW':
    COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_ANTIALIAS '
    COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_OPENGL_HW_ANTIALIAS '
elif OPENGL_ANTIALIAS == 'SW':
    COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_ANTIALIAS '

if compile_helper.get_value('NATIVE_WINDOW_BORDERLESS', False) :
    COMMON_CCFLAGS=COMMON_CCFLAGS+' -DNATIVE_WINDOW_BORDERLESS=1 '

if compile_helper.get_value('NATIVE_WINDOW_NOT_RESIZABLE', False) :
    COMMON_CCFLAGS=COMMON_CCFLAGS+' -DNATIVE_WINDOW_NOT_RESIZABLE=1 '


COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_MBEDTLS=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS+' -DENABLE_CURSOR=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_TEXT_BIDI=1 '
#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DLOAD_ASSET_WITH_MMAP=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_DATA_READER_WRITER=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_EVENT_RECORDER_PLAYER=1 '
COMMON_CCFLAGS = COMMON_CCFLAGS + \
    ' -DWITH_ASSET_LOADER -DWITH_FS_RES -DWITH_ASSET_LOADER_ZIP '
COMMON_CCFLAGS = COMMON_CCFLAGS + \
    ' -DSTBTT_STATIC -DSTB_IMAGE_STATIC -DWITH_STB_IMAGE '
COMMON_CCFLAGS = COMMON_CCFLAGS + \
    ' -DWITH_VGCANVAS -DWITH_UNICODE_BREAK -DWITH_DESKTOP_STYLE '
COMMON_CCFLAGS = COMMON_CCFLAGS + \
    ' -DWITH_SDL -DHAS_STDIO -DHAVE_STDIO_H -DHAS_GET_TIME_US64 '
COMMON_CCFLAGS = COMMON_CCFLAGS+' -DHAS_STD_MALLOC -DTK_MAX_MEM_BLOCK_NR=3 '

#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DTK_MAX_MEM_BLOCK_NR=3 '
#COMMON_CCFLAGS=COMMON_CCFLAGS+' -DWITH_FAST_LCD_PORTRAIT '

COMMON_CCFLAGS = COMMON_CCFLAGS+ ' -DWITH_RES_TOOLS'

COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_MAIN_LOOP_CONSOLE=1 '

GRAPHIC_BUFFER = 'default'

if INPUT_ENGINE == 't9':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_T9 '
elif INPUT_ENGINE == 't9ext':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_T9EXT'
elif INPUT_ENGINE == 'pinyin':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_PINYIN '
elif INPUT_ENGINE == 'spinyin':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_SPINYIN '
elif INPUT_ENGINE == 'null':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_IME_NULL '

if LCD == 'SDL_GPU':
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_GPU -DWITH_VGCANVAS_LCD'
    COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_STB_FONT '
elif LCD == 'SDL_FB_MONO':
    NANOVG_BACKEND = 'AGGE'
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_LCD_MONO '
    COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_STB_FONT '
else:
    COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_STB_FONT '
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_BITMAP_BGRA -DWITH_NANOVG_SOFT '
    if FRAME_BUFFER_FORMAT == 'bgra8888':
        COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_FB_BGRA8888=1 '
    else:
        COMMON_CCFLAGS = COMMON_CCFLAGS+' -DWITH_FB_BGR565=1'

if VGCANVAS == 'CAIRO':
    NANOVG_BACKEND_LIBS = ['cairo', 'pixman']
    NANOVG_BACKEND_PROJS = ['3rd/cairo/SConscript', '3rd/pixman/SConscript']
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_VGCANVAS_CAIRO -DHAVE_CONFIG_H -DCAIRO_WIN32_STATIC_BUILD '
    COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_BITMAP_PREMULTI_ALPHA -DWITH_PIXMAN_G2D -DWITH_G2D -DWITHOUT_DEFAULT_G2D '

elif VGCANVAS == 'NANOVG_PLUS':
    NANOVG_BACKEND_LIBS = ['nanovg_plus']
    NANOVG_BACKEND_PROJS = ['3rd/nanovg_plus/SConscript']
    NANOVG_BACKEND_CPPPATH = [joinPath(
        TK_3RD_ROOT, 'nanovg_plus/gl'), joinPath(TK_3RD_ROOT, 'nanovg_plus/base')]
    COMMON_CCFLAGS = COMMON_CCFLAGS + \
        ' -DWITH_NANOVG_PLUS_GPU -DWITH_NANOVG_GPU -DWITH_GPU_GL '
    if NANOVG_BACKEND == 'GLES2':
        COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_GPU_GLES2 -DNVGP_GLES2 '
    elif NANOVG_BACKEND == 'GLES3':
        COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_GPU_GLES3 -DNVGP_GLES3 '
    else:
        COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_GPU_GL3 -DNVGP_GL3 '
else:
    NANOVG_BACKEND_PROJS = ['3rd/nanovg/SConscript']
    NANOVG_BACKEND_CPPPATH = [
        joinPath(TK_3RD_ROOT, 'nanovg'),
        joinPath(TK_3RD_ROOT, 'nanovg/gl'),
        joinPath(TK_3RD_ROOT, 'nanovg/base'),
        joinPath(TK_3RD_ROOT, 'nanovg/agge'),
        joinPath(TK_3RD_ROOT, 'nanovg/bgfx')]
    if NANOVG_BACKEND == 'AGG':
        NANOVG_BACKEND_LIBS = ['nanovg-agg', 'nanovg', 'agg']
        NANOVG_BACKEND_PROJS += ['3rd/agg/SConscript']
        COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_NANOVG_AGG '
    elif NANOVG_BACKEND == 'AGGE':
        NANOVG_BACKEND_LIBS = ['nanovg-agge', 'nanovg', 'agge']
        NANOVG_BACKEND_PROJS += ['3rd/agge/SConscript']
        COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_NANOVG_AGGE '
    elif NANOVG_BACKEND == 'BGFX':
        NANOVG_BACKEND_LIBS = ['nanovg-bgfx', 'nanovg', 'bgfx']
        NANOVG_BACKEND_PROJS += ['3rd/bgfx/SConscript']
        COMMON_CCFLAGS = COMMON_CCFLAGS + ' -DWITH_NANOVG_BGFX -DWITH_NANOVG_GPU '
    elif NANOVG_BACKEND == 'GLES2':
        NANOVG_BACKEND_LIBS = ['nanovg']
        COMMON_CCFLAGS = COMMON_CCFLAGS + \
            ' -DWITH_NANOVG_GLES2 -DWITH_GPU_GLES2 -DWITH_NANOVG_GL -DWITH_NANOVG_GPU  '
    elif NANOVG_BACKEND == 'GLES3':
        NANOVG_BACKEND_LIBS = ['nanovg']
        COMMON_CCFLAGS = COMMON_CCFLAGS + \
            ' -DWITH_NANOVG_GLES3 -DWITH_NANOVG_GL -DWITH_NANOVG_GPU  '
    elif NANOVG_BACKEND == 'GL2':
        NANOVG_BACKEND_LIBS = ['nanovg']
        COMMON_CCFLAGS = COMMON_CCFLAGS + \
            ' -DWITH_NANOVG_GL2 -DWITH_NANOVG_GL -DWITH_NANOVG_GPU  '
    else:
        NANOVG_BACKEND_LIBS = ['nanovg']
        COMMON_CCFLAGS = COMMON_CCFLAGS + \
            ' -DWITH_NANOVG_GL3 -DWITH_NANOVG_GL -DWITH_NANOVG_GPU  '

BUILD_DEBUG_FLAG = ''
BUILD_DEBUG_LINKFLAGS = ''
OS_PROJECTS = []
OS_WHOLE_ARCHIVE = toWholeArchive(AWTK_STATIC_LIBS)
AWTK_DLL_DEPS_LIBS = AWTK_STATIC_LIBS + \
    NANOVG_BACKEND_LIBS + ['SDL2', 'glad'] + OS_LIBS


if OS_DEBUG :
    BUILD_DEBUG_FLAG = ' -g -O0 '
else :
    BUILD_DEBUG_FLAG = ' -Os '

if OS_NAME == 'Darwin':
    OS_WHOLE_ARCHIVE = ' -all_load '
elif OS_NAME == 'Linux':
    OS_PROJECTS = ['3rd/SDL/SConscript']
elif OS_NAME == 'Windows':
    OS_PROJECTS = ['3rd/SDL/SConscript']
    OS_LINKFLAGS += '\"' + WIN32_AWTK_RES + '\" '

    if TOOLS_NAME == 'mingw':
        print('mingw')
    else:
        OS_WHOLE_ARCHIVE = ' /DEF:"dllexports/awtk.def" '
        if OS_DEBUG :
            BUILD_DEBUG_FLAG = '  /MDd -D_DEBUG -DDEBUG /DEBUG /Od '
        else :
            BUILD_DEBUG_FLAG = ' -DNDEBUG /MD /O2 /Oi  '
        
        if compile_helper.get_value('PDB', True) :
            BUILD_DEBUG_LINKFLAGS = ' /DEBUG '


CFLAGS = COMMON_CFLAGS
LINKFLAGS = OS_LINKFLAGS
LIBPATH = [TK_BIN_DIR, TK_LIB_DIR] + OS_LIBPATH
CCFLAGS = OS_FLAGS + COMMON_CCFLAGS
AWTK_CCFLAGS = OS_FLAGS + COMMON_CCFLAGS

AWTK_STATIC_LIBS = AWTK_STATIC_LIBS + NANOVG_BACKEND_LIBS 
STATIC_LIBS = AWTK_STATIC_LIBS + ['SDL2', 'glad'] + OS_LIBS
SHARED_LIBS = ['awtk'] + OS_LIBS

LIBS = STATIC_LIBS

CPPPATH = [TK_ROOT,
           TK_SRC,
           TK_3RD_ROOT,
           joinPath(TK_SRC, 'ext_widgets'),
           joinPath(TK_SRC, 'custom_widgets'),
           joinPath(TK_3RD_ROOT, 'fribidi'),
           joinPath(TK_3RD_ROOT, 'mbedtls/include'),
           joinPath(TK_3RD_ROOT, 'mbedtls/3rdparty/everest/include'),
           joinPath(TK_3RD_ROOT, 'pixman'),
           joinPath(TK_3RD_ROOT, 'cairo'),
           joinPath(TK_3RD_ROOT, 'bgfx/bgfx/include'),
           joinPath(TK_3RD_ROOT, 'bgfx/bx/include'),
           joinPath(TK_3RD_ROOT, 'bgfx/bimg/include'),
           joinPath(TK_3RD_ROOT, 'agge'),
           joinPath(TK_3RD_ROOT, 'agg/include'),
           joinPath(TK_3RD_ROOT, 'SDL/src'),
           joinPath(TK_3RD_ROOT, 'SDL/include'),
           joinPath(TK_3RD_ROOT, 'agge/src'),
           joinPath(TK_3RD_ROOT, 'agge/include'),
           joinPath(TK_3RD_ROOT, 'gpinyin/include'),
           joinPath(TK_3RD_ROOT, 'libunibreak'),
           joinPath(TK_3RD_ROOT, 'gtest/googletest'),
           joinPath(TK_3RD_ROOT, 'gtest/googletest/include'),
           TK_TOOLS_ROOT] + OS_CPPPATH + NANOVG_BACKEND_CPPPATH

CUSTOM_WIDGETS_SRC=glob.glob('src/custom_widgets/*/src') + \
  glob.glob('src/custom_widgets/*/src/*/src') + \
  glob.glob('src/custom_widgets/*/src/*/include')

for iter in CUSTOM_WIDGETS_SRC:
  fullpath = os.path.join(TK_ROOT, str(iter));
  CPPPATH.append(fullpath);

os.environ['LCD'] = LCD
os.environ['TK_ROOT'] = TK_ROOT
os.environ['CCFLAGS'] = CCFLAGS + BUILD_DEBUG_FLAG
os.environ['VGCANVAS'] = VGCANVAS
os.environ['TOOLS_NAME'] = TOOLS_NAME
os.environ['GTEST_ROOT'] = GTEST_ROOT
os.environ['TK_3RD_ROOT'] = TK_3RD_ROOT
os.environ['INPUT_ENGINE'] = INPUT_ENGINE
os.environ['NANOVG_BACKEND'] = NANOVG_BACKEND
os.environ['NATIVE_WINDOW'] = NATIVE_WINDOW
os.environ['GRAPHIC_BUFFER'] = GRAPHIC_BUFFER
os.environ['FRAME_BUFFER_FORMAT'] = FRAME_BUFFER_FORMAT
os.environ['OS_WHOLE_ARCHIVE'] = OS_WHOLE_ARCHIVE
os.environ['AWTK_DLL_DEPS_LIBS'] = ';'.join(AWTK_DLL_DEPS_LIBS)
os.environ['STATIC_LIBS'] = ';'.join(STATIC_LIBS)

os.environ['WITH_AWTK_SO'] = 'true'
os.environ['AWTK_CCFLAGS'] = AWTK_CCFLAGS + BUILD_DEBUG_FLAG
os.environ['CROSS_COMPILE'] = str(not TOOLS_PREFIX == '')

os.environ['SDL_UBUNTU_USE_IME'] = str(compile_helper.get_value('SDL_UBUNTU_USE_IME', False))
# os.environ['SDL_UBUNTU_USE_IME'] = str(True)
OS_LIBS = ['SDL2', 'glad'] + OS_LIBS

def genIdlAndDef():
    genIdlAndDefEx(True)
