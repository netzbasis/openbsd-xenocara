! $OpenBSD: Xresources.cpp,v 1.13 2015/03/16 20:41:21 matthieu Exp $
!
!
!
!
!


#define BS \ /* cpp can be trickier than m4 */
#define NLBS \n\ /* don't remove these comments */
xlogin*login.translations: #override BS
	Ctrl<Key>R: abort-display()NLBS
	<Key>F1: set-session-argument(failsafe) finish-field()NLBS
	<Key>Left: move-backward-character()NLBS
	<Key>Right: move-forward-character()NLBS
	<Key>Home: move-to-begining()NLBS
	<Key>End: move-to-end()NLBS
	Ctrl<Key>KP_Enter: set-session-argument(failsafe) finish-field()NLBS
	<Key>KP_Enter: set-session-argument() finish-field()NLBS
	Ctrl<Key>Return: set-session-argument(failsafe) finish-field()NLBS
	<Key>Return: set-session-argument() finish-field()

xlogin*greeting: CLIENTHOST
xlogin*namePrompt: \040\040\040\040\040\040\040Login:
xlogin*fail: Login incorrect or forbidden

XHASHif WIDTH > 800
xlogin*greetFont: -adobe-helvetica-bold-o-normal--24-240-75-75-p-138-iso8859-1
xlogin*font: -adobe-helvetica-medium-r-normal--18-180-75-75-p-98-iso8859-1
xlogin*promptFont: -adobe-helvetica-bold-r-normal--18-180-75-75-p-103-iso8859-1
xlogin*failFont: -adobe-helvetica-bold-r-normal--18-180-75-75-p-103-iso8859-1
xlogin*greetFace:       DejaVu Sans Condensed-22:bold:italic:dpi=75
xlogin*face:            DejaVu Sans Condensed-16:dpi=75
xlogin*promptFace:      DejaVu Sans Condensed-16:bold:dpi=75
xlogin*failFace:        DejaVu Sans Condensed-16:bold:dpi=75
XHASHelse
xlogin*greetFont: -adobe-helvetica-bold-o-normal--17-120-100-100-p-92-iso8859-1
xlogin*font: -adobe-helvetica-medium-r-normal--12-120-75-75-p-67-iso8859-1
xlogin*promptFont: -adobe-helvetica-bold-r-normal--12-120-75-75-p-70-iso8859-1
xlogin*failFont: -adobe-helvetica-bold-o-normal--14-140-75-75-p-82-iso8859-1
xlogin*greetFace:       DejaVu Sans Condensed-18:bold:italic:dpi=75
xlogin*face:            DejaVu Sans Condensed-12:dpi=75
xlogin*promptFace:      DejaVu Sans Condensed-12:bold:dpi=75
xlogin*failFace:        DejaVu Sans Condensed-12:bold:dpi=75
XHASHendif

XHASHif !(defined(bpp1) || defined(bpp4) || defined(bpp8) || defined(bpp15))
XHASH if PLANES < 4
XHASH  ifndef bpp1
XHASH   define bpp1
XHASH  endif
XHASH else
XHASH  if PLANES > 4
XHASH   if PLANES > 8
XHASH    ifndef bpp15
XHASH     define bpp15
XHASH    endif
XHASH   else
XHASH    ifndef bpp8
XHASH     define bpp8
XHASH    endif bpp8
XHASH   endif
XHASH  else
XHASH   ifndef bpp4
XHASH    define bpp4
XHASH   endif
XHASH  endif
XHASH endif
XHASHendif  //**/* If manual override */**//

XHASHifndef bpp1
xlogin*borderWidth: 1
xlogin*frameWidth: 2
xlogin*innerFramesWidth: 0

xlogin.Login.sepWidth: 0

! top/left border
xlogin*hiColor: #667788

! bottom/right border
xlogin*shdColor: #667788

! 'Welcome to..' text color
xlogin*greetColor: #fef886
XHASHif defined(bpp4) || defined(bpp8) || defined(bpp15)
! flood fill
xlogin*background: #798a99
xlogin*inpColor: #99aab9
XHASHendif
xlogin*failColor: #aa0000

! 'Login:' and 'Password:'
*Foreground: #eeeeff

! border/shadow
*Background: #000000
XHASHelse
xlogin*borderWidth: 3
xlogin*frameWidth: 5
xlogin*innerFramesWidth: 1
xlogin*shdColor: white
xlogin*hiColor: white
xlogin*greetColor: white
xlogin*background: black
xlogin*failColor: white
xlogin*promptColor: white
*Foreground: white
*Background: black
XHASHendif
XHASHifdef bpp1
xlogin*logoFileName: BITMAPDIR/**//OpenBSD_1bpp.xpm
XHASHendif
XHASHifdef bpp4
xlogin*logoFileName: BITMAPDIR/**//OpenBSD_4bpp.xpm
XHASHendif
XHASHifdef bpp8
xlogin*logoFileName: BITMAPDIR/**//OpenBSD_8bpp.xpm
XHASHendif
XHASHifdef bpp15
xlogin*logoFileName: BITMAPDIR/**//OpenBSD_15bpp.xpm
XHASHendif

xlogin*useShape: true
xlogin*logoPadding: 10

! uncomment to disable logins
! xlogin.Login.allowRootLogin:	false

XConsole*background:	black
XConsole*foreground:	white
XConsole*borderWidth:	2
XConsole*borderColor:   grey
XConsole.text.geometry:	480x130
XConsole.verbose:	true
XConsole*iconic:	true
XConsole*font:		fixed

Chooser*geometry:		640x480
Chooser*allowShellResize:	false
Chooser*viewport.forceBars:	true

Chooser*label.font:	  -adobe-helvetica-bold-o-normal--24-*-p-*-iso8859-1
Chooser*label.label:	  XDMCP Host Menu from CLIENTHOST
Chooser*label.foreground: black
Chooser*list.font:	  lucidasanstypewriter-12
Chooser*Command.font:	  -adobe-helvetica-medium-r-normal--18-*-p-*-iso8859-1
