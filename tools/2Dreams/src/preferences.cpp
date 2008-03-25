/*
 * Copyright (C) 2003-2006 Intel Corporation
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 */

/****************************************************************************
** Form implementation generated from reading ui file 'preferences.ui'
**
** Created: Tue Feb 3 16:18:24 2004
**      by: The User Interface Compiler ($Id: preferences.cpp 341 2006-10-31 12:19:54Z cjbeckma $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#include "preferences.h"

#include <qvariant.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qframe.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qpushbutton.h>
#include <qslider.h>
#include <qspinbox.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qimage.h>
#include <qpixmap.h>

static const char* const image0_data[] = { 
"32 32 376 2",
".a c None",
".# c None",
"Qt c None",
"aR c #000000",
"dW c #000008",
"cr c #000010",
"dM c #001041",
".I c #001ccd",
".9 c #001ce6",
".g c #0038cd",
".m c #0044cd",
"ba c #080400",
"bN c #080800",
"aS c #080808",
"cs c #080c10",
".U c #0828ee",
".n c #082cee",
".x c #0834de",
".w c #0838e6",
".l c #0844d5",
"br c #100c08",
"aM c #10184a",
"#G c #101c41",
"a6 c #102052",
"#H c #102873",
".H c #102ce6",
".T c #1034e6",
".8 c #1034ee",
"cA c #10388b",
"#. c #1038f6",
".s c #104cd5",
".h c #1050f6",
"cP c #181400",
"cy c #181800",
"dH c #181c00",
"#X c #182862",
"bG c #182c52",
"dV c #18388b",
"#0 c #183c73",
".J c #184cff",
".o c #1855f6",
".V c #1859e6",
"#7 c #201c20",
"ak c #202020",
"dS c #202408",
"bZ c #20449c",
".W c #204ca4",
".K c #205dcd",
".y c #205dee",
"#I c #205dff",
".i c #2061cd",
".A c #2061d5",
".v c #2061e6",
"## c #2065e6",
"#v c #2065ff",
".d c #206df6",
"aX c #294c83",
"dI c #294c94",
"dh c #294c9c",
"cQ c #295094",
"c7 c #2950b4",
"cb c #2955a4",
"ci c #2955ac",
".S c #2955ee",
"#F c #2959de",
"#T c #2959ee",
"#h c #2959f6",
"dZ c #312c10",
"dT c #314cac",
"aj c #315dee",
".q c #3161b4",
"#u c #3161ff",
"#k c #3165ac",
"by c #3165c5",
"ax c #3165de",
".7 c #3165ff",
"#g c #3169ff",
"cV c #3171f6",
".N c #3175e6",
".t c #3175ee",
"a5 c #3175f6",
".B c #3175ff",
"#U c #392039",
"ca c #392c00",
"d1 c #393c18",
"cW c #39487b",
"dw c #394cac",
".p c #3961bd",
"bU c #3969cd",
".M c #3969d5",
"#f c #3969ff",
"ab c #396dbd",
".4 c #396dff",
".6 c #3971ff",
".Z c #3979f6",
"#t c #3979ff",
"#c c #3981de",
"cz c #415db4",
".G c #415dff",
".R c #4171ff",
"#r c #4175ff",
".F c #4179ff",
".c c #417dff",
".Q c #4181ff",
"ds c #4a4820",
"aq c #4a7dd5",
"#6 c #4a7dff",
"bF c #4a85f6",
".u c #4a85ff",
"#n c #4a89ee",
".2 c #4a89ff",
".D c #4a8dff",
".P c #4a91ff",
"be c #523808",
"do c #524800",
"c2 c #525000",
"#s c #5279ff",
".5 c #527dff",
".e c #5281de",
".f c #5285e6",
"aw c #528dff",
"aK c #5291ff",
".0 c #5295ff",
"av c #5299ff",
"bT c #5a4010",
"cj c #5a4400",
"cX c #5a4800",
".j c #5a81de",
"#S c #5a81ff",
"#J c #5a85ff",
"#q c #5a89ff",
"#z c #5a8dde",
".3 c #5a8dff",
"#B c #5a91de",
"cU c #5a91f6",
"aL c #5a95ff",
".O c #5a99ff",
"#e c #5a9dff",
"cB c #624000",
"db c #624808",
"dN c #625d08",
"dn c #625d73",
"d0 c #626120",
"a# c #62615a",
"ao c #626162",
"da c #626173",
"aF c #626562",
".Y c #6285de",
".k c #6291ff",
"aY c #6295f6",
".E c #6295ff",
"#l c #6299f6",
".1 c #6299ff",
"dj c #629df6",
"a4 c #629dff",
"ai c #62a1ff",
"#d c #62a5ff",
"bx c #6a4008",
"dB c #6a6d7b",
"dU c #6a8dff",
"#b c #6a91de",
"dy c #6a91ff",
".b c #6a99f6",
"c8 c #6a99ff",
".r c #6a9dee",
"bl c #6a9dff",
".C c #6aa1ff",
"#a c #6aa5f6",
"#E c #6aa5ff",
"#p c #6aaaff",
"aW c #733c08",
"dC c #736110",
"dt c #737120",
"#i c #7389ee",
"#w c #7399ff",
"bE c #73a1ff",
".z c #73a5ff",
".X c #73aaff",
"#5 c #73aeff",
"#C c #73b6ff",
"aE c #7b5918",
"cH c #7b7120",
"c3 c #7b7900",
"dx c #7b9dff",
"dJ c #7ba1ff",
"cR c #7ba5ff",
".L c #7baaf6",
"#y c #7baaff",
"#D c #7bb2ff",
"a. c #836520",
"dX c #837520",
"cS c #83a5ee",
"bW c #83aaff",
"cf c #83aef6",
"bY c #83aeff",
"#o c #83b2ff",
"aJ c #83b6ff",
"ag c #83baff",
"b0 c #8b6500",
"an c #8b6520",
"dk c #8b9dff",
"di c #8ba1ff",
"#m c #8baae6",
"bk c #8baeff",
"ah c #8bb6ff",
"#R c #8bbaff",
"#Q c #8bbeff",
"au c #8bc2ff",
"de c #948910",
"#L c #94b6ff",
"bD c #94beff",
"at c #94c2ff",
"#W c #9c7131",
"bm c #9c7518",
"a7 c #9c7520",
"bH c #9c7910",
"bM c #9c894a",
"b# c #9c8962",
"dK c #9cb2ee",
"bB c #9cbef6",
"a3 c #9cbeff",
"a2 c #9cc2ff",
"#4 c #9cc6ff",
"cq c #a49539",
"c9 c #a4b6ff",
"d. c #a4baf6",
"cg c #a4baff",
"cc c #a4beff",
"ch c #a4c2ff",
"as c #a4c6ff",
"#P c #a4caff",
"aN c #ac8129",
"aB c #ac8d7b",
"cI c #ac9920",
"aC c #aca18b",
"dz c #acbaff",
"#A c #acc6ff",
"bj c #accaff",
"ae c #acceff",
"aG c #acd2ff",
"b6 c #b49552",
"bq c #b4956a",
"cT c #b4c6ff",
"bA c #b4caff",
"bi c #b4ceff",
"#O c #b4d6ff",
"#M c #b4daff",
"bC c #bdd6ff",
"#N c #bddaff",
"#K c #bddeff",
"bV c #c5d6ff",
"a1 c #c5daff",
"ad c #c5deff",
"#j c #c5e2ff",
"ay c #cda152",
"#8 c #cdaa62",
"aQ c #cdae94",
"dL c #cdceff",
"dl c #cdd2e6",
"bX c #cddaff",
"aI c #cddeff",
"bz c #cde2ff",
"#V c #d59d5a",
"dr c #d5d231",
"#Z c #d5deff",
"af c #d5e2ff",
"#2 c #d5e6ff",
"al c #dea55a",
"c6 c #deba00",
"c1 c #ded600",
"aa c #dedeff",
"bh c #dee6ff",
"ce c #deeaff",
"#3 c #deeeff",
"ar c #def2ff",
"cx c #e6a508",
"cO c #e6b200",
"dg c #e6c600",
"dv c #e6ce00",
"dG c #e6d610",
"dR c #e6da20",
"d# c #e6daee",
"aZ c #e6eaff",
"#1 c #e6f2ff",
"df c #eede08",
"dY c #eee231",
"dm c #eee2ee",
"ap c #eeeaff",
"a0 c #eef2ff",
"cd c #eef6ff",
"ck c #f6c600",
"b7 c #f6ca5a",
"bs c #f6ca83",
"bS c #f6d25a",
"bd c #f6d27b",
"bO c #f6d283",
"bb c #f6d6a4",
"cY c #f6de00",
"aA c #f6e6c5",
"dA c #f6eaf6",
"dp c #f6f208",
"ac c #f6f2ff",
"bg c #f6f6ff",
"bf c #f6faff",
"c# c #ffce39",
"cC c #ffd200",
"bw c #ffd26a",
"cw c #ffd620",
"aT c #ffd6ac",
"ct c #ffda4a",
"b9 c #ffda6a",
"cN c #ffde00",
"cm c #ffde39",
"cu c #ffde4a",
"b2 c #ffde6a",
"bQ c #ffde7b",
"cE c #ffe208",
"cl c #ffe220",
"cv c #ffe241",
"b3 c #ffe262",
"bJ c #ffe27b",
"bu c #ffe294",
"aV c #ffe29c",
"cM c #ffe610",
"cL c #ffe618",
"bP c #ffe67b",
"bo c #ffe694",
"a9 c #ffe6ac",
"cD c #ffea00",
"cG c #ffea29",
"co c #ffea52",
"bK c #ffea7b",
"c. c #ffea83",
"bc c #ffeaac",
"c5 c #ffee00",
"cn c #ffee52",
"b1 c #ffee62",
"b4 c #ffee7b",
"bI c #ffee8b",
"bR c #ffee94",
"bp c #ffeea4",
"aD c #ffeeb4",
"c0 c #fff200",
"cJ c #fff229",
"bn c #fff29c",
"bt c #fff2a4",
"bv c #fff2ac",
"c4 c #fff600",
"cF c #fff620",
"cK c #fff629",
"b5 c #fff66a",
"b8 c #fff67b",
"a8 c #fff6b4",
"aP c #fff6c5",
"am c #fff6de",
"#Y c #fff6ff",
"dO c #fffa4a",
"bL c #fffa94",
"#9 c #fffade",
"aH c #fffaff",
"cZ c #ffff00",
"dc c #ffff08",
"dd c #ffff10",
"du c #ffff29",
"dq c #ffff31",
"dF c #ffff41",
"cp c #ffff5a",
"dE c #ffff6a",
"dQ c #ffff73",
"dD c #ffff7b",
"dP c #ffffac",
"b. c #ffffc5",
"aO c #ffffd5",
"aU c #ffffde",
"az c #ffffee",
"#x c #ffffff",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#.b.c.d.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.a.e.fQt.g.h.iQt.aQt.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#.j.k.l.mQt.#.n.o.p.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.a.q.r.s.t.u.v.w.aQt.x.y.aQt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.z.A.B.C.D.E.F.G.HQt.I.J.KQt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.L.M.N.O.P.D.Q.c.F.R.S.T.a.U.V.W.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.X.Y.Z.z.0.1.2.3.F.F.4.5.6.7.8.9#.##.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt#a#b#c#d#e.0.P.2.Q.F.6.4#f#f#g.4#h.n#i#j#kQt.aQt",
"Qt.#Qt.#Qt.#Qt#l#m#n#o#p.z.0.1.2#q#r#s.4#t.7.R#u#v#w#x#x#x#yQt.#",
".aQt.aQt.aQt#z#A#B#C#D#E.O.0.D.2.c#r.6#r#F#G#H#I#J#x#x#x#x#x#KQt",
"Qt.#Qt.##L#M#N#O#P#Q#R#E#E.0.1.u#q#r#S#T#U#V#W#X#Y#x#x#x#x#x#Z#0",
".aQt.a.C#1#2#P#3#4#4#5#E.O.0.D.2.c#6#F#7#8#x#9a.a##x#x#x#xaaabQt",
"Qt.#.bac#xadaeafaeagahai.C.0.1.u.3ajakal#x#x#xamanao#x#xapaqQt.#",
".aaqar#x#1as#P#jatau.X.Cav.0.Dawax#7ayazaAaBaCazaDaEaFac#zQt.aQt",
"QtaG#xaHaI#o#3aGaeaJaJai.CaKaL.BaMaNaOaPaQaRaSaTaUaVaWaXQt.#Qt.#",
".aaY#xaZ#oa0#ja1a2a3.za4aw.Da5a6a7a8a9b.b#aRbabbbcaPbdbe.aQt.aQt",
"Qt.#ad#Lbfbgbhbibj#obk.1bl.Q#XbmbnbobpbnbqaRbrbsbtbubvbwbx.#Qt.#",
".abybzbAbBbgbzbCbDagbE.1bFbGbHbIbJbKbJbLbMaRbNbObPbKbQbRbSbT.aQt",
"bUbh#xaHbVbWbXa1#AbYbYa4bZb0b1b2bKb3b4b5b6aRaSb7b8b3bKb9c.c#ca.#",
"cbcc#1#xcdcecfcgbicha4#ecicjckclcmcncocpcqcrcsctcncncucvcwcxcyQt",
"Qt.#czbkcd#Ycda2bkcc#AaLaLcAcBcCcDcEcFcFcGcHcIcJcKcLcMcNcOcPQt.#",
".aQt.aQtcQ#obzcd#OcRcScTcUcVcWcXcYcZc0cZc1c2c3cZc4c4c5c6cPQt.aQt",
"Qt.#Qt.#Qt.#c7cR#2afchc8c9d.d#dadbcDdcdddeaRaRdfdccZdgcPQt.#Qt.#",
".aQt.aQt.aQt.aQtdhdi#O#NdjdkdldmdndodpdqdrdsdtdudddvcyQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#dwdxadaJdydzdAdBdCcKdDdEdDdFdGdHQt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQtdIdJ#4.EdKdLdMdNdOdPdQdRdSQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#dTdxdUdVQtdWdXdOdYdZQt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.ad0d1Qt.aQt.aQt.aQt.aQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt.aQt"};

static const char* const image1_data[] = { 
"16 16 158 2",
"Qt c None",
"#H c #000cbd",
"#u c #000cc5",
"#T c #0010ac",
"#v c #0010b4",
"aB c #0014ac",
"#h c #0014b4",
"#g c #0018c5",
"#S c #0018cd",
"#G c #001ccd",
"#R c #0024d5",
"#Q c #0028de",
"ao c #0030bd",
".4 c #0820bd",
"#t c #0824cd",
".P c #0828bd",
"#F c #0830e6",
"ag c #0834cd",
"#8 c #083ccd",
"#P c #083cee",
"an c #1028b4",
"aA c #1030ac",
".j c #1048cd",
"#7 c #182cbd",
"#s c #184ce6",
"#E c #1850ee",
"az c #203cb4",
"ay c #2040b4",
"#f c #2059ee",
".f c #205dd5",
".3 c #205dee",
"#r c #2065f6",
"ax c #2948bd",
"aw c #294cbd",
"#O c #2965f6",
"av c #3159cd",
".O c #3159de",
"#U c #3165de",
"#D c #316dff",
"#e c #3181ff",
"#6 c #3948cd",
"af c #3950c5",
"#5 c #3955d5",
"au c #395dcd",
".N c #3961d5",
"ap c #3969d5",
".Z c #3969de",
"#k c #3971de",
"#d c #3985ff",
"#4 c #4161e6",
"at c #4165d5",
"as c #4169d5",
"#3 c #4169e6",
"aq c #416dd5",
"ar c #416dde",
"#i c #417dde",
"#q c #4181ff",
".1 c #4191ff",
".L c #4a71de",
"#I c #4a81de",
".Q c #4a81e6",
".h c #4a85ee",
"#N c #4a85ff",
"#C c #4a89ff",
".2 c #4a99ff",
".o c #5285e6",
".i c #5285ee",
"#V c #5289e6",
".6 c #5291ee",
".e c #5a65bd",
".Y c #5a7dde",
"#2 c #5a81ee",
".g c #5a8dee",
"#J c #5a91e6",
".0 c #5a99ff",
"#M c #5a9dff",
".z c #626dcd",
".M c #6289ee",
".7 c #628de6",
".# c #6a75c5",
"#x c #6a91ee",
"#b c #6a99f6",
"#c c #6aa5ff",
".a c #7379c5",
".t c #7381d5",
".S c #7391e6",
"#9 c #7399ee",
"#a c #73a5ff",
"#B c #73aeff",
".K c #7b95e6",
"#1 c #7ba1f6",
".s c #7baef6",
".X c #83a1e6",
"## c #83b2f6",
"#p c #83baff",
"#w c #83c2f6",
"#L c #83ceff",
".b c #8b89d5",
".H c #8b95de",
".W c #8ba5ee",
".V c #8baaee",
"#0 c #8bb2ff",
".U c #94aeee",
".T c #94b2f6",
".R c #94bef6",
"#. c #94beff",
"#A c #94c6ff",
".I c #9cbaee",
".C c #9cbef6",
"#Z c #9cc2ff",
".r c #9cdaff",
".n c #a4a1e6",
".D c #a4aade",
"#K c #a4c2f6",
".9 c #a4c6ff",
".k c #acaae6",
".8 c #acc6ff",
"#z c #acdaff",
"#o c #acdeff",
".q c #ace2ff",
".B c #ace6ff",
"am c #b4aed5",
"ae c #b4b2d5",
"ah c #b4c6f6",
"#Y c #b4deff",
"al c #bdb2d5",
"ad c #bdbade",
"#W c #bddaff",
".p c #bdeeff",
"ak c #c5c2de",
"ac c #c5c2e6",
".J c #c5c6e6",
"#j c #c5f6ff",
".G c #cdd2f6",
"#X c #cdeeff",
".A c #cdffff",
"aj c #d5cae6",
"ab c #d5d2ee",
".c c #d5d2f6",
".u c #d5d6f6",
"#m c #d5e6ff",
".5 c #d5ffff",
"aa c #ded6ee",
"#l c #deeeff",
"#n c #deffff",
"ai c #e6daee",
".y c #e6deff",
"a# c #e6e2f6",
".d c #e6e2ff",
"#y c #e6ffff",
".x c #eee6ff",
"a. c #eeeaf6",
".l c #eeeaff",
".F c #eeeeff",
".m c #f6f2ff",
".w c #fff6ff",
".E c #fffaff",
".v c #ffffff",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQt.#.aQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.b.c.d.eQtQtQtQt",
".f.g.h.i.jQtQt.k.l.m.l.nQtQtQtQt",
".o.p.q.r.s.t.u.v.w.x.x.y.zQtQtQt",
".o.A.B.C.D.m.v.E.F.l.d.G.HQtQtQt",
".o.A.I.J.v.v.v.v.E.w.K.L.M.N.O.P",
".Q.A.R.S.T.U.V.W.X.Y.Z.0.1.2.3.4",
".Q.5.6.7.8.9#.###a#b#c#d#e#f#g#h",
"#i#j#k#l#m#n#o#p.0#q#r#s#t#u#vQt",
".Q#w#x#y#z#A#B#C#D#E#F#G#g#H#hQt",
"#I#J#K.p#L#c#M#N#O#P#Q#R#S#H#TQt",
"#U#V#W#X#Y#Z#0#1#2#3#4#5#6#7QtQt",
"#8#9.v.v.v.wa.a#aaabacadaeafQtQt",
"agah.v.v.v.v.Ea.aiajakalamanQtQt",
"aoapaqarasatauavawaxayazaAaBQtQt"};

static const char* const image2_data[] = { 
"32 32 185 2",
"Qt c None",
"#t c #393c39",
"aA c #39658b",
"aG c #396594",
".9 c #413c41",
".V c #414041",
"#J c #414441",
"aw c #416594",
"at c #416994",
"aW c #4a444a",
".t c #4a484a",
"aY c #4a4c4a",
"#1 c #4a6d94",
"#E c #525052",
"a1 c #525552",
"aV c #526d8b",
"ab c #52719c",
"aR c #52759c",
"#G c #5a595a",
"a2 c #5a5d5a",
"#u c #5a6162",
"aT c #5a7da4",
"aU c #5a7dac",
"aS c #5a81ac",
"#0 c #5a85b4",
"ai c #625d62",
".k c #626562",
"aO c #6281ac",
"au c #6289bd",
"aQ c #6a6562",
"#i c #6a656a",
"#a c #6a696a",
".X c #6a6d6a",
"aJ c #6a6d73",
"an c #6a8dbd",
"#U c #6a91bd",
".4 c #736d73",
"#D c #737173",
"#m c #73717b",
"#N c #737573",
".K c #73757b",
"#g c #73797b",
"#9 c #7391bd",
"af c #7395bd",
"#b c #7b757b",
"#A c #7b797b",
".5 c #7b7d7b",
"#Y c #7baed5",
"#Z c #7bb2de",
"az c #837d83",
".l c #838183",
".N c #838583",
"#c c #83858b",
".Q c #83898b",
"#W c #83b2d5",
"#X c #83b2de",
"aN c #83b6e6",
".Y c #8b858b",
".D c #8b898b",
"#p c #8b8d8b",
"aa c #8b9194",
"#V c #8bbae6",
"aM c #8bc2ee",
".2 c #948d94",
".d c #949194",
".h c #949594",
"#L c #94999c",
"al c #94cae6",
".L c #9c959c",
"av c #9c999c",
".i c #9c9da4",
"ar c #9ccede",
"ac c #9ccee6",
"#F c #a49da4",
".c c #a4a1a4",
"#j c #a4a5a4",
".j c #a4a5ac",
".E c #a4aaac",
"#6 c #a4d2e6",
"as c #a4d2ee",
"#5 c #a4d6e6",
"aq c #a4d6ee",
"aB c #a4d6f6",
"#7 c #a4dae6",
"aH c #a4daee",
".g c #aca5ac",
".b c #acaaac",
"#f c #acaeac",
"#I c #acaeb4",
"ax c #acd6e6",
"ae c #acd6ee",
"#8 c #acd6f6",
"#4 c #acdaee",
"ak c #acdaf6",
"ay c #b4aeb4",
".a c #b4b2b4",
".1 c #b4b6b4",
"#H c #b4b6bd",
"am c #b4d6ee",
"ad c #b4daee",
"#3 c #b4deee",
"ap c #bdb6b4",
".f c #bdb6bd",
"#x c #bdbabd",
"aj c #bdbebd",
"#B c #bdbec5",
"#C c #bdc2c5",
"aI c #bddeee",
"#2 c #bde2ff",
".e c #c5c2c5",
".# c #c5c6cd",
"#w c #cdc2c5",
"aP c #cdcacd",
"a0 c #cdcecd",
"#o c #cdced5",
".s c #d5cecd",
"#K c #d5ced5",
".y c #d5d2d5",
".7 c #d5d6d5",
".O c #d5d6de",
"#n c #ded6de",
".P c #dedade",
".v c #dedede",
".x c #dedee6",
"#q c #e6d6d5",
"#M c #e6dee6",
".M c #e6e2e6",
"aZ c #e6e6e6",
"aX c #e6e6ee",
"#z c #eed2d5",
"aL c #eed6d5",
"a. c #eedede",
".Z c #eee6ee",
".0 c #eeeaee",
".8 c #eeeeee",
"ao c #eeeef6",
"#R c #eef2f6",
"#P c #f69583",
"#Q c #f6bab4",
"#l c #f6d2cd",
"#y c #f6d6cd",
"aF c #f6dede",
"#O c #f6eaee",
"#h c #f6eeee",
"#S c #f6eef6",
".u c #f6f2f6",
".w c #f6f6f6",
"#v c #f6f6ff",
"aD c #f6ffff",
"## c #ff856a",
".G c #ff8573",
"#d c #ff8973",
".J c #ff8d7b",
".S c #ff9183",
"ah c #ff9d8b",
"ag c #ffa194",
"aC c #ffa594",
"aK c #ffa59c",
"aE c #ffaa9c",
"#r c #ffb2a4",
".T c #ffb6a4",
"#T c #ffb6ac",
"#e c #ffbaac",
"a# c #ffbab4",
"#s c #ffbeb4",
".U c #ffc2b4",
"#k c #ffc6bd",
"#. c #ffcac5",
".I c #ffcec5",
".F c #ffd2cd",
".n c #ffd6cd",
".3 c #ffd6d5",
".r c #ffdad5",
".z c #ffded5",
".B c #ffdede",
".H c #ffe2de",
".C c #ffe6e6",
".W c #ffeae6",
".R c #ffeaee",
".A c #ffeeee",
".q c #fff2f6",
".m c #fff6f6",
".6 c #fff6ff",
".o c #fffaff",
".p c #ffffff",
"QtQt.#.a.b.c.dQtQtQtQtQtQt.e.f.g.c.hQtQtQtQtQtQt.c.b.c.i.j.kQtQt",
".l.m.n.o.p.q.r.s.tQtQtQt.u.v.w.p.w.x.y.kQtQtQt.s.z.A.p.p.B.C.DQt",
".E.F.G.H.p.I.J.r.KQtQt.L.M.N.O.p.P.Q.O.dQtQtQt.R.S.T.p.m.J.U.e.V",
".h.p.W.p.p.p.W.q.XQtQt.Y.p.Z.o.p.p.0.u.DQtQtQt.u.A.m.p.p.W.o.1.V",
".2.p.p.A.3.o.p.u.4QtQt.5.p.p.6.7.6.p.o.DQtQtQt.8.p.p.3.W.p.p.a.9",
".N.p.p#.##.H.p.u#aQtQt#b.p.p.7#c.y.p.w.DQtQtQt.8.p.q#d#e.p.p#f.V",
".l.p.p.6.A.o.p.u.4QtQt.X.p.p.o.Z.o.p.6.DQtQtQt.0.p.p.A.m.p.p.b.9",
"#g.m.H.o.p.q.C#h#aQtQt#i.w.Z.w.p.w.Z.8.DQtQtQt.Z.C.A.p.o.H.m#j.9",
"#g#..G.r.p#k#d#l#mQtQt.k#n#c.y.p#n.Q#o#pQtQtQt#q.S#r.p.A.J#s.b#t",
"#u#q.F.q#v.R.n#w.kQtQt.V.P.P.8.p.u.P.y.DQtQtQt#x#y.C.w.w.3#z#A.V",
".l#x#B#x#x#x#C.1#DQtQtQt#E.c.g#F#j#j.2#GQtQtQt#x#H.1.a#I.a#f.d#J",
"#p.p.w#K#L#M.w.o#NQtQtQtQtQtQtQtQtQtQtQtQtQtQt.o.w#O#P#Q#R.p.1.V",
".Y.p.p.P.b.0.p.u#bQtQtQtQtQtQtQtQtQtQtQtQtQtQt#S.p.6#T.F.p.p.a.9",
"#A.p.p.p.o.p.p.u#DQtQtQt#U#V#W#X#Y#Z#0QtQtQtQt.8.p.p.p.p.p.p#f.V",
".5.M#x.0.p.x#B.M#AQtQt#1#2#3#4#5#6#7#8#9QtQtQta.a#.I.p.q#e.3#H.V",
"#D.yaa#M.p.y.d.P#NQtQtab#3#5acacacadaeafQtQtQt#qag#s.p.qah#.#I.V",
"ai.7#S.8.u.8#Saj#iQtQt#1akalacalamadaeanQtQtQt#I.0.0ao.0#S.P.N#J",
".N.e#x#xapaj#x#x#aQtQt#1aqacaraead#3asanQtQtQt#K#xaj.1#x#x.e.c.V",
".2.p.p.I.S.H.p.o.4QtQtatakacaeadadadaeauQtQtQt.o.p.uav#B.p.p.f.9",
".l.p.p.H#T.A.p.u#aQtQtawaqacaxadaxadasauQtQtQt#S.p.o#B.P.p.pay.V",
"az.p.p.p.p.p.p#R.4QtQtaAaB#6adadadadae#0QtQtQt.0.p.p.p.p.p.p.a.V",
"#g.raC.CaD.3aEaF.XQtQtaGaHadad#3adaIae#0QtQtQt#M.E.#.o.u.j#K.a.V",
"aJ.naK.C.p.3aEaLaJQtQtQtaMaq#8asaeaqaNaOQtQtQt#n#xaP.p.w.a.yay#J",
"aQ#x.y.7.y.y#o.b.kQtQtQtQtaRaSaSaTaUaVaWQtQtQt.4.y#M#n.P.P.e.NQt",
".daX.y.e.baP.O.M#AQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.t#EaY#E.tQtQt",
".N.p.p.y.Q.Z.p.o#NQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
".l.p.p.6.0.o.p.u#bQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"#A.oaZ.o.p.w.0#S#NQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"aza0.Q.P.paP.D.7#AQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"a1.M.7.w.p#S.7#K.XQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"Qta2.cavavav#F#N.VQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt"};

static const char* const image3_data[] = { 
"16 16 91 2",
"Qt c None",
"#d c #393c5a",
"## c #39486a",
".S c #394873",
"#y c #4a4462",
".M c #5271a4",
".Z c #5279a4",
".G c #5281bd",
"#u c #5a2c10",
".R c #5a5d7b",
"#c c #5a6583",
"#a c #5a6d8b",
".4 c #5a7da4",
".B c #5a7db4",
".L c #626983",
"#b c #627594",
".s c #62759c",
".l c #6291cd",
"#t c #6a3008",
"#q c #6a3010",
"#w c #6a4029",
"#x c #6a4c39",
".3 c #6a6983",
".8 c #6a6d83",
".9 c #6a6d94",
".F c #6a7594",
".e c #6a99d5",
".t c #6abaf6",
"#e c #73697b",
".f c #73baf6",
".a c #7ba5d5",
".b c #7ba5de",
".T c #7bc6f6",
".m c #7bcaff",
".g c #7bceff",
".i c #7bd2ff",
"#p c #833c08",
"#m c #834410",
"#. c #83757b",
".r c #8399b4",
".# c #8399bd",
".k c #83a5c5",
".j c #83cef6",
".h c #83d2ff",
".N c #83daff",
".d c #8b99b4",
".c c #8baacd",
".H c #8bd6ff",
".I c #8bdeff",
"#l c #944808",
".p c #94daff",
".q c #94deff",
".u c #94e2ff",
".A c #9cd6e6",
".5 c #9ce2f6",
".U c #9ce2ff",
".n c #9ce6ff",
".0 c #9ceeff",
".J c #a4e6ff",
".y c #a4eeff",
"#h c #ac5508",
"#f c #ac7941",
".x c #aceaff",
".D c #aceeff",
".o c #acf2ff",
".z c #acfaff",
".Y c #b4c2cd",
".V c #b4faff",
".6 c #b4ffff",
".O c #bdfaff",
".E c #bdffff",
".7 c #c5d6e6",
".v c #c5ffff",
"#i c #cd9541",
"#r c #cdaa62",
".K c #cdffff",
"#g c #d57d18",
"#o c #d58118",
"#n c #d5ae62",
".w c #d5ffff",
"#k c #de8118",
".C c #defaff",
".P c #deffff",
".Q c #e6ffff",
"#v c #eeb25a",
".2 c #eef2f6",
".W c #eeffff",
".1 c #f6ffff",
"#j c #ffda7b",
"#s c #ffde7b",
".X c #ffffff",
"QtQtQt.#.a.b.c.dQtQtQtQtQtQtQtQt",
"QtQt.e.f.g.h.i.j.kQtQtQtQtQtQtQt",
"Qt.l.m.n.o.n.p.q.n.rQtQtQtQtQtQt",
".s.t.u.v.w.x.o.y.z.AQtQtQtQtQtQt",
".B.g.y.w.C.D.v.E.v.E.FQtQtQtQtQt",
".G.H.I.J.D.v.K.K.K.w.LQtQtQtQtQt",
".M.N.p.n.O.K.w.P.Q.P.RQtQtQtQtQt",
".S.T.H.U.V.v.w.W.X.YQtQtQtQtQtQt",
"Qt.Z.0.y.V.v.P.1.2.3QtQtQtQtQtQt",
"QtQt.4.5.6.K.w.7.8.9#.QtQtQtQtQt",
"QtQtQt###a#b#c#dQt#e#f#g#hQtQtQt",
"QtQtQtQtQtQtQtQtQtQt#i#j#k#lQtQt",
"QtQtQtQtQtQtQtQtQtQt#m#n#j#o#pQt",
"QtQtQtQtQtQtQtQtQtQtQt#q#r#s#o#t",
"QtQtQtQtQtQtQtQtQtQtQtQt#u#n#v#w",
"QtQtQtQtQtQtQtQtQtQtQtQtQt#u#x#y"};

static const char* const image4_data[] = { 
"16 16 21 1",
". c None",
"a c #292829",
"e c #292c29",
"s c #312c31",
"# c #6a696a",
"b c #736d73",
"r c #aca5ac",
"q c #acaaac",
"n c #b4b2b4",
"o c #b4b6b4",
"m c #bdb6bd",
"k c #c5c6c5",
"l c #cdc6cd",
"j c #cdcacd",
"p c #d5d2d5",
"h c #d5d6d5",
"g c #dedade",
"i c #e6e2e6",
"f c #eeeaee",
"c c #f6f6f6",
"d c #fff6ff",
"................",
"................",
"................",
"................",
"................",
".......#a.......",
"......bcde......",
".....#fghga.....",
"....bijkjlje....",
"...#gmnmomnma...",
"..#pqrqrqrqrqe..",
"..esaeasesasae..",
"................",
"................",
"................",
"................"};

static const char* const image5_data[] = { 
"16 16 18 1",
". c None",
"f c #312c31",
"# c #6a696a",
"b c #6a6d6a",
"a c #736d73",
"p c #949194",
"o c #949594",
"n c #a49da4",
"m c #a4a1a4",
"l c #acaeac",
"k c #b4aeb4",
"j c #b4b2b4",
"h c #c5c6c5",
"i c #cdc6cd",
"g c #cdcacd",
"d c #dedade",
"c c #dedede",
"e c #e6dee6",
"................",
"................",
"................",
"................",
"..##a###a#b#a#..",
"..#cdedecedccf..",
"...#ghihghgif...",
"....#jkjljkf....",
".....#mnmnf.....",
"......#opf......",
".......#f.......",
"................",
"................",
"................",
"................",
"................"};

static const char* const image6_data[] = { 
"16 16 37 1",
". c None",
"F c #000473",
"D c #00047b",
"C c #00087b",
"y c #000c83",
"x c #00108b",
"E c #00148b",
"H c #001494",
"w c #00188b",
"B c #001894",
"u c #001c94",
"I c #001c9c",
"s c #00209c",
"r c #0024a4",
"o c #0028a4",
"k c #002cac",
"G c #0030ac",
"j c #0030b4",
"n c #0034bd",
"g c #0038bd",
"q c #0038c5",
"i c #003cc5",
"d c #0040c5",
"f c #0040cd",
"m c #0044cd",
"a c #0048d5",
"v c #004cd5",
"c c #004cde",
"# c #0050de",
"b c #0055e6",
"A c #0059e6",
"z c #0059ee",
"e c #005dee",
"t c #0061f6",
"h c #0065f6",
"p c #0069ff",
"l c #006dff",
"....#a..........",
"...bcad.........",
"..eb..fg........",
".he...dijk......",
"lh....mgnko.....",
"ph.....qj.rs....",
".te.....nk.su...",
".h.bv....kr.wx..",
".he.#a....os.xy.",
"..zA.ad....sB.CD",
"...b#.mg....BE.F",
"....va.qj...xE.D",
".....mm.nG..HE.F",
"......di.kIuEH.F",
".......gg......F",
"........jkoswxCD"};

static const char* const image7_data[] = { 
"16 16 84 2",
"Qt c None",
".D c #5a5983",
".u c #6a698b",
"#r c #833c08",
".L c #836552",
".B c #837d7b",
".r c #83819c",
".w c #8381a4",
"#o c #8b4008",
".v c #8b8dac",
"#k c #944410",
".n c #9491ac",
".C c #9495bd",
".i c #9c99b4",
".q c #a49db4",
".5 c #ac5908",
".t c #aca5bd",
"#b c #b45508",
".M c #b46108",
".e c #b4aec5",
".l c #b4b2cd",
".3 c #bd5500",
".V c #bd5d08",
".8 c #bd6d08",
".6 c #c56900",
"#h c #c56d00",
"#j c #c56d08",
".K c #c5b64a",
".m c #c5becd",
".g c #c5c2d5",
".k c #c5c6d5",
"#q c #cd6100",
"#n c #cd6500",
"#m c #cd6508",
"#d c #cd6d08",
".W c #cd7908",
"#a c #cd8110",
".a c #cdcad5",
".# c #cdcade",
".p c #cdcede",
"#p c #d56500",
".U c #d56900",
"#g c #d58508",
".Y c #d58510",
".h c #d5d2de",
"#c c #de6d00",
".N c #de7900",
"#f c #de8108",
".x c #de8910",
".A c #de8d10",
".c c #dedae6",
".f c #dedaee",
".d c #dedee6",
"#i c #e67d08",
".E c #e68908",
"#e c #e68d10",
".s c #e6e2ee",
"#l c #ee7900",
".7 c #ee8d08",
".F c #ee9508",
".G c #ee9510",
".9 c #eea108",
".T c #eea510",
".Z c #eeaa10",
".y c #eeb608",
".b c #eeeaf6",
".4 c #f67500",
".S c #f6ae08",
"#. c #f6b210",
".2 c #f6ba10",
".Q c #f6be10",
".z c #f6c610",
"## c #ffb208",
".H c #ffba10",
".X c #ffc210",
".R c #ffc610",
".O c #ffd210",
".J c #ffd218",
".1 c #ffd610",
".I c #ffd618",
".0 c #ffee10",
".P c #ffee18",
".o c #fff6ff",
".j c #ffffff",
"QtQtQtQtQtQtQtQtQt.#QtQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.a.bQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQt.c.dQtQtQt.e.fQt",
"QtQtQtQtQtQtQtQt.g.hQtQt.i.j.kQt",
"QtQtQtQtQtQtQtQt.l.mQt.n.o.pQtQt",
"QtQtQtQtQtQtQtQt.i.q.r.s.tQtQtQt",
"QtQtQtQtQtQtQtQt.u.v.#.wQtQtQtQt",
"QtQtQt.x.y.z.AQt.B.C.DQtQtQtQtQt",
"QtQt.E.F.G.H.I.J.K.LQtQtQtQtQtQt",
"Qt.M.NQtQtQt.O.P.Q.R.S.TQtQtQtQt",
"Qt.U.VQtQt.W.X.Y.Z.0.J.1.2QtQtQt",
"Qt.3.4.5.6.7.8Qt.9#.QtQt###aQtQt",
"QtQt#b#c#dQtQtQt#e#fQtQt.Y#gQtQt",
"QtQtQtQtQtQtQtQt#h#iQtQt.N#jQtQt",
"QtQtQtQtQtQtQtQt#k#l#m#n#lQtQtQt",
"QtQtQtQtQtQtQtQtQt#o#p#q#rQtQtQt"};

static const char* const image8_data[] = { 
"32 32 366 2",
".g c None",
".# c None",
"Qt c None",
"aD c #009900",
"aC c #009d00",
"am c #08a508",
"ao c #08a510",
"al c #08aa08",
"an c #10b210",
"aX c #18a110",
"aE c #20a110",
"#E c #20b239",
"#1 c #29b229",
"cb c #311c08",
"cW c #312008",
"as c #3169ff",
"aJ c #3969f6",
"a. c #3971ff",
"#P c #3979ff",
"#3 c #39b639",
"#2 c #39be39",
"dn c #412008",
"c5 c #412c08",
"a# c #4171f6",
"#N c #4171ff",
"#4 c #41c241",
"dg c #4a2c08",
"aW c #4aa520",
"#0 c #4ab229",
"#U c #522810",
".G c #522c18",
"b2 c #523010",
"dI c #523408",
"dt c #523410",
"da c #523810",
"aa c #5279ff",
"#9 c #527df6",
"#O c #527dff",
"dM c #5a3808",
"dL c #5a3810",
"bs c #5a3c18",
"#Q c #5a81ff",
"c0 c #623010",
"ae c #623c20",
"a3 c #624020",
"at c #6285f6",
"#v c #6285ff",
"#t c #628dff",
"#j c #62ba4a",
"#o c #62be4a",
"#e c #6a2c18",
"dR c #6a3810",
".a c #6a3820",
"dh c #6a3c18",
".h c #6a3c20",
"#f c #6a4020",
"cL c #6a4410",
"aK c #6a89ee",
"#s c #6a8dff",
"#u c #6a91ff",
".L c #6ac65a",
".f c #733018",
".F c #733420",
"cx c #733810",
".e c #733c20",
".c c #734020",
".X c #734029",
"bQ c #734418",
"#A c #734420",
"ar c #7385ff",
"#G c #73ca73",
".r c #7b3c20",
"#V c #7b4820",
"ad c #7b4c29",
"#5 c #7bb241",
"#I c #7bce7b",
"#J c #7bd283",
"#F c #7bd683",
".b c #834020",
".i c #835031",
"aU c #83aa31",
"#K c #83b652",
"#H c #83d683",
".d c #8b4439",
"dF c #8b4c10",
"df c #8b5018",
"cc c #8b5020",
"dm c #8b5918",
"co c #8b5d20",
"#z c #8b5d39",
"cU c #8b6129",
"di c #8b6529",
"aT c #8bb231",
"#m c #8bd683",
"#k c #8bd68b",
"cn c #945918",
"cR c #946529",
".s c #946539",
"aI c #9491e6",
".Q c #94a5e6",
"#n c #94ca73",
".H c #9c5931",
"do c #9c6920",
"br c #9c6931",
"cF c #9c6d31",
".E c #9c7141",
".j c #9c7541",
".8 c #9caeff",
".U c #9cb2ff",
"bF c #a45d29",
"dy c #a46920",
"cy c #a46929",
"#D c #a4be4a",
"## c #a4beff",
".1 c #a4c252",
"#i c #a4c641",
"dN c #ac6d18",
"cT c #ac6d20",
"cM c #ac6d31",
".q c #ac6d41",
"a2 c #ac7d4a",
"#d c #ac8d62",
".C c #acb2ee",
".9 c #acc2ff",
".K c #acd262",
"#l c #ace2ac",
"bg c #b46920",
"dQ c #b47518",
"aM c #b47529",
"aL c #b4794a",
".p c #b49162",
".W c #b49562",
"#r c #b4babd",
"aV c #b4be5a",
"#a c #b4beff",
"ap c #b4c662",
"cE c #bd6d29",
"dq c #bd7120",
"bG c #bd7539",
"bf c #bd7941",
"dc c #bd7d20",
".D c #bdaaac",
"#. c #bdc6ff",
".3 c #bdd6a4",
".2 c #bdeabd",
"d# c #c58120",
"ca c #c5814a",
"dO c #c58520",
"c6 c #c58529",
"aY c #c5c262",
"aB c #c5c641",
".A c #c5cad5",
".B c #c5caff",
"ab c #c5cebd",
"dH c #cd7d29",
"cV c #cd8529",
".Y c #cd8941",
"b1 c #cd9952",
".k c #cd995a",
".l c #cdae6a",
"#b c #cdc2de",
".R c #cdd2ff",
"bR c #d58920",
"dl c #d58d31",
".I c #d5a552",
"ak c #d5be4a",
".S c #d5e2ff",
"ax c #de8131",
"cQ c #de9139",
"cw c #de994a",
"dj c #dea15a",
".t c #deaa6a",
"#T c #deb68b",
"#Z c #dec24a",
".n c #dec27b",
".u c #dec662",
"a0 c #decaac",
"#p c #ded273",
"#w c #ded2cd",
".V c #ded6d5",
"dw c #e68d29",
"dB c #e69520",
"dk c #e69d41",
"#g c #e69d4a",
"ds c #e6a139",
"cK c #e6a55a",
"a4 c #e6aa29",
"b. c #e6be52",
".o c #e6c283",
"bb c #e6ca6a",
".M c #e6d273",
"aH c #e6d694",
".T c #e6e6ff",
"bz c #ee6129",
"dC c #ee8d29",
"dv c #ee9520",
"dP c #ee9941",
"dD c #ee9d29",
"de c #ee9d39",
"dd c #ee9d4a",
"dK c #eeaa39",
"af c #eeae41",
".m c #eec67b",
"cD c #eece83",
".7 c #eed2a4",
"aZ c #eedabd",
"bE c #eede9c",
"ac c #eee2a4",
"cg c #f60000",
"bV c #f61c08",
"a9 c #f66d29",
"ci c #f69531",
"#W c #f6a541",
"du c #f6a54a",
"dr c #f6ae39",
"#B c #f6ae5a",
"cP c #f6b231",
"cX c #f6b25a",
"db c #f6b66a",
"dz c #f6ba5a",
"c8 c #f6c662",
"bP c #f6ca94",
"#h c #f6d25a",
"cm c #f6d283",
"#M c #f6debd",
"au c #f6e2bd",
"#R c #f6eed5",
"bJ c #ff0000",
"bI c #ff0c00",
"by c #ff0c18",
"bv c #ff1820",
"bu c #ff1c10",
"bw c #ff1c18",
"bx c #ff2020",
"b7 c #ff2410",
"bK c #ff2810",
"b6 c #ff4818",
"bn c #ff4c29",
"cf c #ff5518",
"bL c #ff5920",
"a8 c #ff595a",
"bU c #ff6518",
"bm c #ff6973",
"cs c #ff6d20",
"b5 c #ff7118",
"bi c #ff7529",
"a7 c #ff7562",
"bj c #ff757b",
"cr c #ff7d18",
"bk c #ff7d83",
"ay c #ff8120",
"bt c #ff8520",
"aO c #ff858b",
"b8 c #ff8931",
"bl c #ff899c",
"bh c #ff9939",
"aP c #ff9d41",
"c4 c #ffa131",
"aN c #ffa531",
"aR c #ffa541",
"b3 c #ffaa29",
"c3 c #ffae18",
"cq c #ffae20",
"cd c #ffae29",
"dG c #ffae4a",
"bM c #ffae52",
"b4 c #ffb220",
"cH c #ffb229",
"ch c #ffb231",
"dE c #ffb239",
"a6 c #ffb2b4",
"dp c #ffb618",
"cN c #ffb620",
"bH c #ffb629",
"a5 c #ffb631",
"ce c #ffba20",
"cO c #ffba29",
"cz c #ffba31",
"ct c #ffba39",
"bS c #ffbe29",
"dA c #ffbe31",
"cB c #ffbe39",
"aQ c #ffbe41",
"d. c #ffbe5a",
"bT c #ffc229",
"cI c #ffc231",
"cA c #ffc239",
"cp c #ffc241",
"dx c #ffc24a",
"dJ c #ffc252",
"c2 c #ffc631",
"az c #ffc641",
"bW c #ffc64a",
"cv c #ffc652",
"cS c #ffc66a",
"cG c #ffca39",
"cC c #ffca41",
"ag c #ffca4a",
"ai c #ffca52",
"bX c #ffca5a",
"c9 c #ffca62",
"c1 c #ffca6a",
"cu c #ffce41",
"#X c #ffce52",
"aA c #ffce5a",
".Z c #ffce62",
"cZ c #ffce7b",
"cj c #ffd252",
"bo c #ffd25a",
"ah c #ffd262",
"aS c #ffd26a",
"c7 c #ffd652",
"#Y c #ffd65a",
"#C c #ffd662",
"bB c #ffd66a",
"bN c #ffd673",
"bY c #ffda62",
"b# c #ffda6a",
"aj c #ffda73",
"b9 c #ffda7b",
".4 c #ffda83",
"aq c #ffda94",
"bA c #ffde5a",
"ck c #ffde6a",
"bp c #ffde7b",
"ba c #ffde83",
"aF c #ffde8b",
"bC c #ffe273",
".0 c #ffe27b",
"#6 c #ffe283",
"bO c #ffe28b",
".J c #ffe673",
"bZ c #ffe67b",
"cY c #ffe683",
".5 c #ffe68b",
"c. c #ffe694",
"#L c #ffe69c",
"#8 c #ffe6b4",
"cl c #ffea83",
"#7 c #ffea8b",
"bd c #ffea94",
"cJ c #ffea9c",
"av c #ffeaac",
"bD c #ffee8b",
".O c #ffee94",
"bq c #ffee9c",
"bc c #ffeea4",
".P c #ffeeb4",
".w c #fff294",
".v c #fff29c",
".N c #fff2a4",
"c# c #fff2ac",
"aG c #fff68b",
"be c #fff69c",
".6 c #fff6a4",
"#q c #fff6ac",
".y c #fffaa4",
"b0 c #fffaac",
"a1 c #fffab4",
".z c #fffabd",
".x c #ffffb4",
"#x c #ffffbd",
"#y c #ffffc5",
"aw c #ffffcd",
"#c c #ffffd5",
"#S c #ffffde",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.a.b.c.d.e.f.#Qt.#Qt.#Qt.#Qt.#",
".gQt.gQt.gQt.gQt.gQt.gQt.gQt.h.i.j.k.l.m.n.o.p.q.cQt.gQt.gQt.gQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#.r.s.t.u.v.w.x.y.z.A.B.C.D.E.F.#Qt.#Qt.#",
".gQt.gQt.gQt.gQt.gQt.G.H.I.J.K.L.M.N.O.P.Q.R.S.T.U.V.W.a.gQt.gQt",
"Qt.#Qt.#Qt.#Qt.#Qt.X.Y.Z.0.1.2.3.4.5.6.7.8.9#.###a#b#c#d#e.#Qt.#",
".gQt.gQt.gQt.gQt#f#g#h#i#j#k#l#m#n#o#p#q#r#s#t#u#v#w#x#y#zQt.gQt",
"Qt.#Qt.#Qt.#Qt#A#B#C#D#E#F#G#H#I#J#K#L.w#M#N#O#N#P#Q#R#S#T#UQt.#",
".gQt.gQt.gQt#V#W#X#Y#Z#0#1#2#3#4#5#6#7#8#9a.#N#Na#aaab#cacad.gQt",
"Qt.#Qt.#Qtaeafagahaiajakalamanaoap.5.vaqaras#Patauavaw#yaw.sQt.#",
".gQt.gQt.gaxayazag#YaAaBaCaCaDaCaEaF.5aGaHaIaJaK.Naw#x#c#xaL.gQt",
"Qt.#Qt.#aMaNaOaPaQaRaSaTaEaUaVaWaXaY.N.5.y.OaZa0#xa1aw#c#ca2Qt.#",
".gQt.ga3a4a5a6aOa7a8a9#Yb.b#ajbabb.0#6bcbd#qbe.x#q#ya1#c#xbf.gQt",
"Qt.#QtbgbhbibjbkblbmbnbobpaSbaajbdbabq.5.N.Oa1.N#xa1aw#xawbrQt.#",
".gQtbsbtbubvbwbxbwbybzbAaAb#bBbCaj.5#6bD.5#qbqa1.N#ya1#cbEbF.gQt",
"Qt.#bGbHbtbIbJbJbKbLbMaAbNahbpbN.5bpbdbO.Nbd#q.N#x#q#y#cbPbQQt.#",
".gQtbRbSbTbUbJbJbVbWagbobXbYahbaajbZbpbq.5.Nbda1.N#xb0awb1b2.gQt",
"Qt.#b3b4azb5bJb6b7b8#CaibB.Zb9bB#6b9bd#6bqc.#qbqa1#q#yc#cacbQt.#",
".gcccdcebScfcgchcibhbWcjai#CahckbB.5bpclbO.Nbd#q.N#x.xcmcnQt.gQt",
"QtcocpcqbTcrcsctcucpaAcvbBaAajaS#6aj.5#6bq.5.Nbqa1.6#ycwcx.#Qt.#",
".gcyczcecqbTczcAcBcCcp#Xai#YaAckbBbCajbd#6bDbd#qbq#xcDcE.gQt.gQt",
"QtcFcGcqctcHcIczbWbWbpb#bBbXajahbabN.5bpcJ.5.Nbda1.NcKcLQt.#Qt.#",
".gcMazcNcqcOcHcIcPcQcRcwahboaAbYaSbaajbZ#6bq.5.6.vcScTQt.gQt.gQt",
"QtcUbNb4ceb4aQchcVcWQt.#cX#XbNahcY.0bdbpbd#6.N.OcZcVc0.#Qt.#Qt.#",
".gQtc1c2c3cebHc4c5Qt.gQtc6c7aAb#c8c9bN.v.5bD.4d.d#da.gQt.gQt.gQt",
"Qt.#dbbBcNcqbTdcQt.#Qt.#dd#XaSdedfdgdhdidjdkdldmdn.#Qt.#Qt.#Qt.#",
".gQtdo#7bWdpcNdq.gQt.gQtdrc7dsdt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt",
"Qt.#Qtdu#qcpcedvQt.#QtdwaAdxdy.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".gQt.gcLdza1#CdAdBdCdDcAazdE.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt",
"Qt.#Qt.#dFdGc#.v#7aAahboajdHQt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".gQt.gQt.gdId#dJb9bcbOajdKdL.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt",
"Qt.#Qt.#Qt.#QtdMdNdOdPdQdR.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt.gQt"};

static const char* const image9_data[] = { 
"16 16 149 2",
"Qt c None",
".t c #29aaff",
"#W c #31ee4a",
"#X c #31f252",
".s c #39beff",
"as c #522000",
".r c #52b2ff",
"#7 c #52de20",
"#8 c #52ea29",
"ao c #5a2400",
".G c #5ac6ee",
".F c #62d6ff",
".Q c #6a2c00",
"#F c #6a3800",
"a. c #733400",
".0 c #733800",
"#r c #733808",
"af c #7b3000",
"#Y c #7b9908",
"#Z c #833000",
".C c #835529",
"an c #8b3800",
"ap c #8b4000",
"#V c #8b9908",
"ar c #944000",
"#9 c #947500",
"ag c #9c4800",
".E c #9cd6de",
"#P c #ac4c00",
"aq c #ac5000",
"ab c #ac65ff",
"#c c #b45d10",
".4 c #b46918",
".B c #b4814a",
"#Q c #bd6d18",
".e c #bd7529",
".a c #bd8552",
"aa c #bd89ff",
".h c #bdd6f6",
"#E c #c56108",
"a# c #c569bd",
"#q c #c56d10",
".3 c #c57931",
".o c #c5854a",
"#6 c #c58910",
".u c #c5ced5",
"#M c #c5d6a4",
".j c #c5e6ff",
".# c #cd7529",
".O c #cd7920",
".f c #cd8139",
".N c #cd8541",
"#0 c #d57d18",
"#G c #d58929",
".A c #d58d41",
"#3 c #d58da4",
"#L c #d59518",
".5 c #d59941",
".d c #d59962",
".b c #d5a573",
"#N c #d5e2bd",
".i c #d5eeff",
".P c #dea15a",
".D c #dea162",
"#5 c #dea1e6",
".c c #deae7b",
"#O c #e69510",
"#d c #e69d39",
".z c #e6aa6a",
".1 c #e6ae5a",
".p c #e6ae6a",
"#4 c #e6ceff",
"ak c #ee8141",
"aj c #eea594",
"ac c #f67110",
"am c #f67500",
"ai c #f68531",
"ah c #f68908",
"#C c #f6aa39",
"#T c #f6b273",
".g c #f6d2b4",
".H c #f6e2c5",
".k c #f6ffff",
".K c #ff8118",
"al c #ff8500",
"ae c #ff8508",
"ad c #ff8d08",
".I c #ff8d29",
"#K c #ff9510",
"#J c #ff9918",
"#2 c #ff9920",
"#D c #ff9d18",
"#A c #ff9d20",
".J c #ff9d31",
"#I c #ffa520",
"#z c #ffa529",
"#B c #ffa531",
"#S c #ffa539",
"#U c #ffa541",
"#y c #ffaa31",
".v c #ffaa7b",
"#1 c #ffae29",
"#H c #ffae31",
"#R c #ffae39",
"#x c #ffb239",
"#m c #ffb24a",
".x c #ffb28b",
"#w c #ffb639",
"#l c #ffb64a",
"#v c #ffbe4a",
"#k c #ffbe52",
"#n c #ffc218",
"#u c #ffc252",
".W c #ffc273",
"## c #ffc639",
"#j c #ffc65a",
"#. c #ffc66a",
".w c #ffc69c",
"#p c #ffca18",
".V c #ffca83",
"#b c #ffce41",
"#s c #ffce62",
"#i c #ffce6a",
".9 c #ffce7b",
".Z c #ffce8b",
".8 c #ffd27b",
".U c #ffd283",
".X c #ffd29c",
"#t c #ffd66a",
"#h c #ffd673",
".7 c #ffd67b",
".n c #ffd6bd",
"#g c #ffda73",
".6 c #ffda8b",
".R c #ffde94",
".L c #ffdebd",
".2 c #ffe294",
".T c #ffe2a4",
"#f c #ffe683",
".S c #ffe6ac",
".m c #ffeae6",
"#a c #ffee62",
".Y c #ffeed5",
"#o c #fff241",
".M c #fff2c5",
".y c #fff2de",
"#e c #fffa9c",
".q c #ffffee",
".l c #ffffff",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQt.#.a.b.c.d.eQtQtQtQtQt",
"QtQtQt.f.g.h.i.j.k.l.m.n.oQtQtQt",
"QtQt.p.q.l.r.s.t.u.v.w.x.y.zQtQt",
"Qt.A.B.C.D.E.F.G.H.I.J.K.L.M.NQt",
".O.PQtQt.Q.R.S.T.T.U.V.W.X.Y.Z.0",
".1.2.3.4.5.6.6.7.8.8.9#.###a#b#c",
"#d#e#f#g#h#i#j#j#k#k#l#m#n#o#p#q",
"#r#d#s#t#s#u#v#w#x#y#z#A#B#C#D#E",
"QtQt#F#G#v#v#x#H#I#J#K#L#M#N#O#P",
"QtQtQtQt#Q#w#R#z#S#T#U#V#W#X#Y#Z",
"QtQtQtQtQt#0#1#2#3#4#5#6#7#8#9Qt",
"QtQtQtQtQta.#A#Da#aaabacadaeafQt",
"QtQtQtQtQtQtagahaiajakalamanQtQt",
"QtQtQtQtQtQtQtaoapagaqarasQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt"};

static const char* const image10_data[] = { 
"32 32 220 2",
"Qt c None",
"br c #000018",
"aM c #000020",
"bb c #000031",
"bo c #000041",
"#U c #000052",
"bi c #00005a",
"a8 c #000062",
"bw c #00006a",
"bs c #000073",
"a2 c #000429",
"aU c #00045a",
"a1 c #000473",
"bn c #00087b",
"at c #000c5a",
"a5 c #000c7b",
"#H c #00188b",
"a4 c #002094",
"#q c #00249c",
"aA c #00289c",
"#A c #0034ac",
"#G c #0081ff",
"#z c #0085ff",
"#y c #0089ff",
"bj c #080473",
"aB c #080862",
"#d c #080c29",
"aa c #080c6a",
"#4 c #080c73",
"b# c #080c7b",
"aL c #080c83",
"#u c #081041",
"#9 c #08104a",
"ag c #081052",
"#g c #081073",
"ax c #08107b",
"aY c #081083",
"a. c #08146a",
"aX c #081473",
"aR c #081483",
"an c #081883",
"ad c #081c83",
"au c #08208b",
"af c #082894",
"#N c #082c9c",
"as c #0830a4",
"a7 c #0834ac",
"aT c #0838ac",
"ap c #0850c5",
"#T c #0855cd",
"bh c #086de6",
"aK c #0879f6",
"az c #0885ff",
"aI c #0889ff",
"aG c #10188b",
"bx c #101c83",
"#F c #102483",
"#Z c #10248b",
"ao c #102494",
"#k c #10287b",
"#8 c #102894",
".M c #102c73",
".4 c #102c7b",
"ah c #102c9c",
"#0 c #103094",
"#t c #10309c",
"#c c #10349c",
"aS c #103cac",
"ay c #1044bd",
"#B c #104cbd",
".L c #1050bd",
".3 c #1055c5",
".v c #1059c5",
"ae c #1059cd",
"bc c #105dc5",
".Z c #1061cd",
"#5 c #1069de",
"#P c #1075ee",
"a0 c #1079f6",
"av c #107df6",
"bg c #1085ff",
"#S c #1089ff",
"aJ c #108dff",
"b. c #1091ff",
"bz c #182c9c",
"a# c #18309c",
"aF c #18349c",
"a6 c #1834a4",
"#O c #183c9c",
".S c #183ca4",
".F c #1840a4",
"#V c #1840ac",
".9 c #1844ac",
".k c #1848a4",
".A c #1850bd",
".B c #1855b4",
"#h c #1855c5",
"#j c #1859c5",
"#Y c #185dcd",
".b c #1865cd",
"am c #1865d5",
"aw c #1871e6",
"aH c #1881f6",
"ai c #1885ff",
"#R c #1889ff",
"#Q c #188dff",
"#p c #1891ff",
"aQ c #203494",
"bk c #2040ac",
".Y c #2048ac",
".G c #204cac",
".N c #204cb4",
".d c #2050ac",
".q c #2050b4",
"bf c #2050bd",
".w c #2055bd",
".l c #205dbd",
".r c #205dc5",
"#E c #205dcd",
".e c #2061c5",
".a c #2069cd",
"#1 c #2069d5",
".H c #206dde",
"#I c #2075de",
"bq c #2079e6",
"aq c #2091ff",
"ar c #2095ff",
".X c #2944a4",
"bt c #2955bd",
"by c #2959c5",
".c c #295dc5",
".# c #2961c5",
"#e c #2965c5",
".j c #2969c5",
".x c #2979de",
"aZ c #2979e6",
".m c #2985ee",
"bp c #2989f6",
"#x c #2995ff",
"#v c #2999ff",
".8 c #3148a4",
"aN c #316dc5",
"a3 c #3175d5",
"#l c #3179d5",
"a9 c #317dde",
"ba c #318dff",
"#w c #3199ff",
"aj c #319dff",
"aE c #3961bd",
".T c #396dc5",
"aC c #3971c5",
"#3 c #399dff",
"#W c #39a1ff",
"bd c #4181e6",
".E c #418dee",
"#. c #4199ff",
"#2 c #419dff",
"ak c #41a1ff",
"#o c #41a5ff",
".5 c #4a79cd",
"bm c #4a99ff",
"al c #4aa5ff",
"#7 c #4aaaff",
".R c #5295e6",
"#s c #529df6",
"#b c #52a1f6",
"bv c #52a1ff",
"be c #52a5ff",
"bl c #52aaff",
"ab c #52aeff",
"aD c #5a7dcd",
".O c #5aaaf6",
"#M c #5aaaff",
"#6 c #5aaeff",
"ac c #5ab2ff",
".f c #62aaf6",
"bu c #62aeff",
"#C c #62b2ff",
"#n c #62b6ff",
".W c #6a8dcd",
"#m c #6ab6ff",
"#J c #6abaff",
".U c #7399d5",
".C c #73baff",
"#K c #73beff",
"#X c #7bbeff",
"#i c #7bc2ff",
"#L c #83c2ff",
"#D c #83c6ff",
"aW c #8bb6ee",
"#f c #8bbaf6",
".s c #8bc6ff",
".V c #94aade",
"aV c #94c2f6",
"#r c #94caff",
".0 c #94ceff",
".p c #9ccaf6",
"## c #9cceff",
".i c #a4c6ee",
".z c #a4d2ff",
".2 c #a4d6ff",
"aP c #acbee6",
".1 c #acd6ff",
"#a c #acdaff",
"aO c #b4caee",
".I c #b4daff",
".u c #b4deff",
".P c #bddeff",
".K c #bde2ff",
".Q c #c5e2ff",
".J c #c5e6ff",
".7 c #cddeff",
".6 c #cde2ff",
".D c #cde6ff",
".g c #d5eaff",
".y c #deeeff",
".t c #e6f2ff",
".n c #eef6ff",
".o c #f6faff",
".h c #ffffff",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.#.#.a.b.c.dQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.e.f.g.h.i.j.kQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.l.m.n.o.o.h.p.qQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.r.s.t.t.n.n.u.vQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.w.x.y.y.t.t.n.z.AQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.B.C.D.g.g.y.y.E.FQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.G.H.I.J.J.g.g.K.L.MQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt.N.O.P.P.Q.Q.D.R.SQtQt",
"QtQt.T.U.V.W.XQtQtQtQtQtQtQtQtQtQtQtQtQt.Y.Z.0.1.I.I.P.2.3.4QtQt",
"Qt.5.6.y.y.y.7.8QtQtQtQtQtQtQtQtQtQtQtQt.9#.##.z.z#a#a#b#c#dQtQt",
"#e.s.1.z.2.z.1#f#gQtQtQtQtQtQtQtQtQtQt.F#h#i####.z.z.z#j#kQtQtQt",
"#l#m#m#m#n#n#o#p#qQtQtQtQtQtQtQtQtQtQt.S.m.s.s#r#r###s#t#uQtQtQt",
".x#v#w#v#x#y#y#z#AQtQtQtQtQtQtQtQtQt.S#B#C#i#D#D#r#r#E#FQtQtQtQt",
".L#z#G#G#G#G#G#G#HQtQtQtQtQtQtQtQtQt#c#I#J#K#K#i#L#M#N#uQtQtQtQt",
"#O#P#Q#R#Q#S#Q#T#UQtQtQtQtQtQtQtQtQt#V#W#m#m.C.C#X#Y#ZQtQtQtQtQt",
"Qt#0#1#2#3#3#h#4QtQtQtQtQtQtQtQtQt#t#5#6#6#C#C#m#7#8#9QtQtQtQtQt",
"QtQta.#Fa#aa#UQtQtQtQtQtQtQtQtQtQt#t#w#7ababacac.HadQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt#8ae#3#o#o#7#7#oafagQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtahaiaj#3akakalamanQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtaoapaqar#v#v#3#wasatQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtauav#Qaqar#v#vawaxQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtanayaz#S#S#Q#paqaAaBQtQtQtQtQtQtQtQt",
"QtQt.#aCaDaEaFQtQtQtQtQtQtQtaGaH#z#zaIaIaJaKaLaMQtQtQtQtQtQtQtQt",
"QtaNaO.t.t.taPaQQtQtQtQtQtaRaS#x#z#z#z#z#zaTaUQtQtQtQtQtQtQtQtQt",
".#aV.I.I.I.I.IaWaXQtQtQtQtaYaZ#w#xaI#z#za0a1a2QtQtQtQtQtQtQtQtQt",
"a3#X#K#X#K#Xac#Wa4QtQtQta5a6#3#3#w#w#x#xa7a8QtQtQtQtQtQtQtQtQtQt",
"a9#W#W#W#Wb.#y#z#AQtQtQtb#a3alakak#3#3baaLbbQtQtQtQtQtQtQtQtQtQt",
"bcaz#zaz#G#z#G#z#HQtQtQta1bdbebeakak#2bfa8QtQtQtQtQtQtQtQtQtQtQt",
".Fav#Saz#Sbg#SbhbiQtQtQtbjbk#6blblalbmbnboQtQtQtQtQtQtQtQtQtQtQt",
"Qt#Vbp#w#w#wbqa5brQtQtQtQtbsbtbu#Mbv.SbwQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtbx#Vbya6a8brQtQtQtQtQtQtbsb#bzb#bwbrQtQtQtQtQtQtQtQtQtQtQtQt",
"QtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQtQt"};

static const char* const image11_data[] = { 
"32 32 263 2",
".b c None",
".# c None",
"Qt c None",
"#o c #006dff",
"aG c #0079ff",
"as c #0081ff",
"at c #0085ff",
"aH c #0089ff",
"a4 c #0091ff",
"au c #0095ff",
"bu c #0099ff",
"bv c #009dff",
".7 c #00a1ff",
"bb c #00a5ff",
"a5 c #00aaff",
"aT c #00aeff",
"av c #00b2ff",
"aw c #00b6ff",
"ag c #00baff",
"a# c #00beff",
"#p c #00c2ff",
"#2 c #00c6ff",
"#Q c #00caff",
"#A c #00ceff",
".8 c #00d2ff",
".9 c #00d6ff",
".H c #00daff",
".x c #00deff",
".l c #00e2ff",
".v c #00e6ff",
".w c #00eaff",
".m c #00eeff",
"bG c #083c9c",
"bO c #0844b4",
"bR c #0861ee",
"bB c #0869de",
"b8 c #0875de",
"#z c #087dff",
"aS c #0885ff",
"bX c #088dee",
"bh c #0899ff",
"bc c #08a5ff",
"b2 c #08aaff",
"#3 c #08b2ff",
"aa c #08baff",
"#R c #08beff",
"#Z c #08c2ff",
".L c #08c6ff",
"#. c #08d2ff",
".M c #08d6ff",
"#k c #08daff",
"#j c #08deff",
".G c #08e6ff",
"cc c #103cb4",
"bS c #1040bd",
"cd c #1048a4",
"bs c #1048b4",
"bY c #104cde",
"b3 c #1050e6",
"b7 c #1055cd",
"bN c #1059e6",
"bI c #105dd5",
"bF c #105de6",
"bD c #1061ee",
"bi c #1069de",
"bJ c #106dee",
"aY c #1075e6",
"b5 c #1079de",
"aE c #1079ff",
"bU c #107dff",
"bV c #1081de",
"bW c #1081f6",
"br c #1085ff",
"ca c #1089f6",
"aR c #1089ff",
".u c #108dff",
"ap c #1091ff",
".S c #1095ee",
"aN c #1095ff",
"a9 c #1099ff",
"bw c #10a1ff",
"bA c #10a5ff",
"#8 c #10b2ff",
".J c #10baff",
"bK c #10bef6",
".K c #10caff",
".I c #10ceff",
"#w c #10d2ff",
".p c #10d6ff",
"aZ c #184cb4",
"b6 c #184cbd",
"ce c #1850c5",
"b1 c #1850d5",
"aF c #185dc5",
"b0 c #1861d5",
"cb c #1861de",
"b. c #1865ee",
"c. c #187df6",
".6 c #1881ff",
"#q c #1885e6",
"b4 c #1889e6",
"bT c #188de6",
"ax c #189df6",
"c# c #189dff",
"bx c #18a5ff",
"bE c #18b2f6",
"bq c #18b2ff",
"bg c #18b6ff",
"aD c #18baff",
".0 c #18c2ff",
"ao c #18c6ff",
".z c #18caff",
".D c #18d6ff",
".y c #18deff",
".C c #18e2ff",
"aO c #2061e6",
"aq c #2069de",
"b9 c #2071ee",
"ah c #2071f6",
"## c #2075ee",
"#9 c #2085f6",
"aQ c #2091f6",
"bZ c #2095ff",
"ba c #2099ff",
"bk c #20a5ff",
".k c #20b2ff",
"aM c #20c2ff",
".A c #20c6ff",
".q c #20d2ff",
"bQ c #2969bd",
"a. c #296dee",
".U c #2971de",
".T c #2971ee",
".W c #2975d5",
"bL c #2975e6",
"#0 c #297de6",
"bM c #2985e6",
"#M c #298dff",
"aj c #2991ff",
".3 c #2995ee",
".Z c #29a5ff",
".2 c #29b2ff",
"bz c #29b6ff",
".R c #29c2ff",
".V c #3175ee",
"#d c #3179d5",
"#b c #3179e6",
"#m c #317dde",
"#4 c #317de6",
"#r c #317dee",
".X c #317df6",
"#E c #3181c5",
"#G c #3181d5",
"#S c #3181de",
"#c c #3181e6",
"#g c #3181ee",
"#T c #3185cd",
".Y c #3185e6",
"#f c #3185ee",
"#x c #3185ff",
"#u c #3189d5",
"#s c #3189de",
"#l c #318dff",
"aI c #3191ff",
"bt c #3199ee",
"#i c #31a5ff",
"#P c #31b2ff",
"by c #31baff",
"#7 c #31beff",
".f c #31c6ff",
".e c #31d6ff",
"#N c #396dcd",
".4 c #397dee",
"#B c #397df6",
"ay c #3981e6",
".B c #3989de",
"#t c #3989ee",
"bP c #3989f6",
".P c #398dff",
".1 c #3991ee",
"#U c #3995cd",
"#H c #3999ee",
"#C c #3999f6",
"bH c #3999ff",
"#D c #399dee",
"bd c #39a5f6",
"bC c #39aeff",
".N c #39b2ff",
"#a c #417dee",
"#e c #4181ee",
"#F c #4185cd",
"b# c #418dde",
".O c #418df6",
"a1 c #4191de",
"a2 c #4191ee",
"ak c #4191ff",
"aU c #4195ee",
".Q c #4195ff",
"a6 c #419dff",
"#v c #41a1ee",
"#J c #41a1f6",
"#W c #41a5e6",
".r c #41a5ff",
"#Y c #41b2f6",
"#V c #41b2ff",
"#1 c #41b6ff",
".j c #4a9dee",
"a0 c #4a9df6",
"aP c #4aa1ff",
".h c #4aa5f6",
"#y c #4aa5ff",
"#L c #4aaaff",
"#X c #4ab6ff",
".F c #4abaff",
"ac c #4acaf6",
"ab c #4aceff",
"#h c #5299ff",
"#I c #52a1f6",
".i c #52a5ff",
"ar c #52aaf6",
"#O c #52b2f6",
"be c #52b2ff",
".d c #52baff",
"bp c #52c2ff",
"ae c #52c6f6",
"a8 c #52c6ff",
"ad c #52d2ff",
"am c #52daff",
"az c #52def6",
"aB c #52e6ff",
"bj c #5a9dff",
"a3 c #5aa5ff",
"#K c #5aaaff",
"#n c #5ab6f6",
".g c #5ab6ff",
".s c #5abaff",
".t c #5abeff",
"#5 c #5ac2ff",
"af c #5ac6ff",
".E c #5acaff",
"aW c #5ae6ff",
"aJ c #5aeaff",
"aA c #5aeeff",
"aV c #5af2ff",
".o c #62aeff",
".n c #62b2ff",
".c c #62c2ff",
"an c #62c6ff",
"bf c #62caff",
"aL c #62d2ff",
"aC c #62daff",
"ai c #6abaff",
".a c #6abeff",
"#6 c #6ac2ff",
"bn c #6acaff",
"aX c #6adaff",
"al c #6ae2ff",
"aK c #6af2ff",
".5 c #73c2ff",
"bl c #73c6ff",
"a7 c #73e6ff",
"bm c #7bceff",
"bo c #83d6ff",
"Qt.#Qt.#Qt.#Qt.#Qt.#.a.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".bQt.bQt.bQt.b.c.d.e.f.g.bQt.h.i.jQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt",
"Qt.#Qt.#Qt.#Qt.g.k.l.m.f.n.i.o.p.q.rQt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".bQt.bQt.s.c.b.t.u.v.l.w.x.y.z.w.l.A.BQt.bQt.bQt.bQt.bQt.bQt.bQt",
"Qt.#Qt.t.C.D.E.F.G.x.w.H.I.J.K.L.w.M.N.OQt.P.Q.#Qt.#Qt.#Qt.#Qt.#",
".bQt.s.R.H.v.H.v.H.x.S.T.U.V.W.X.Y.Z.L.0.1.2.3.4.bQt.bQt.bQt.bQt",
"Qt.#.5.6.7.8.l.9#.###a#b#c#d#e#b#f#g#h#i#j.9#k#l#m.#Qt.#Qt.#Qt.#",
".bQt#n.6#o#p.8.H#q#r#s#t#u#t#s#t#u#t#s.Q#v#w.L#x.bQt.bQt.bQt.bQt",
"Qt.#.a#y#z#A.9#p#B#C#D#E#F#G#H#C#I#C#J#C#K#L#w#M#N.#Qt.#Qt.#Qt.#",
".b.t#O.s#P.8#Q#R#S#T.bQt.bQt.bQt#U#V#W#V#W#X#Y#Z#0Qt.bQt.bQt.bQt",
"Qt#1#A#2#A#2#A#3#4.#Qt.#Qt.#Qt.#Qt.##5.F#5.F#6#7#8#9#Ba.Qt.#Qt.#",
".b#Pa##Qa##Qa#aa.WQt.bQt.bQt.bQt.bQt.babacadaeafa##Qagah.bQt.bQt",
"aiaj#2a##2a##2a#ak.#Qt.#Qt.#Qt.#Qt.#Qt.#alamalanaoa##2apaq.#Qt.#",
"ar#Masatauavaw#paxay.bQt.bQt.bQt.bQt.bQtazaAaBaCaD#p.7aEaFQt.bQt",
"QtajasaGasaHa#avaaaIQt.#Qt.#Qt.#Qt.#Qt.#QtaJaKaLaMavaNaOQt.#Qt.#",
".baPaQaRaSat.7awaT#8aUQt.bQt.bQt.bQt.bQt.baVaWaX#8awaYaZ.bQt.bQt",
"Qta0a1a2a3a4.7a5awa5#8a6Qt.#Qt.#Qt.#Qt.#QtaJa7a8awa5a9b.Qt.#Qt.#",
".bQt.bQtb#ba.7bbbbbbbbbcbdbe.bQt.bQt.bQt.balbfbgbbbbbbbhbiQt.bQt",
"Qt.#Qt.#bjbaa5.7a5.7a5.7a5bk.s.gbl.cbmbnbobpbq.7a5.7a5.7brbsQt.#",
".bQt.bQtbt.7bubvbubvbubvbubvbubwbxby#PbzbAbvbubvbubba5.7bBQt.bQt",
"Qt.#QtakbCaga5bu.7bbagbv.7bu.7bu.7bu.7bu.7bu.7bua5ag#2bbbD.#Qt.#",
".bQt.bakbE#Aa#bvaw#A#2#Aavbuauauauauauauauauauaubb#AawbFbGQt.bQt",
"Qt.#Qt.#bH#Z.H.8bqbx.M.8.Hav.7au.7au.7au.7au.7auaSbIbJbGQt.#Qt.#",
".bQt.bQt.b#lbKaj.WbLbM.K.H#pauauauau.H.v.H#2auaua4bNbOQt.bQt.bQt",
"Qt.#Qt.#Qt.#bPbQQt.#Qt.Tbga#.7au.7bu.m.l.m.lavau.7bRbS.#Qt.#Qt.#",
".bQt.bQt.bQt.bQt.bQt.bQtbT#AbuauaubUbVbW.H.w.HawbXbY.bQt.bQt.bQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#bZ.l.m.x#Zb0Qtb1aN.l.mb2b3.#Qt.#Qt.#Qt.#",
".bQt.bQt.bQt.bQt.bQt.bQtb4.w.l.lb5b6.bQtb7bcb8bY.bQt.bQt.bQt.bQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#b9c.c#cacb.#Qt.#QtbYcc.#Qt.#Qt.#Qt.#Qt.#",
".bQt.bQt.bQt.bQt.bQt.bQt.bQtcdce.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt",
"Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#Qt.#",
".bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt.bQt"};

static const char* const image12_data[] = { 
"16 16 45 1",
". c None",
"Q c #004800",
"M c #005000",
"D c #005d00",
"z c #006100",
"H c #006d00",
"L c #007100",
"s c #007500",
"P c #007900",
"v c #007d00",
"l c #008100",
"g c #008500",
"o c #008900",
"b c #008d00",
"C c #009500",
"E c #009d00",
"O c #00ae00",
"w c #00b200",
"N c #00b600",
"K c #00be00",
"p c #00c200",
"I c #00ca00",
"A c #00ce00",
"J c #00d200",
"B c #00d600",
"F c #00da00",
"u c #00de00",
"G c #00e200",
"x c #00ea00",
"t c #00f200",
"y c #00ff00",
"c c #088d08",
"d c #089108",
"# c #089900",
"i c #089908",
"a c #089d00",
"m c #08b208",
"n c #08f208",
"q c #08f608",
"e c #10a110",
"r c #10c210",
"h c #18ba18",
"j c #18ce18",
"f c #20aa20",
"k c #20be20",
"................",
"................",
"..............#a",
"b............bb.",
"cd.........efg..",
".hi......djkl...",
".mno...gpqrs....",
".gtuvswxywz.....",
".zAtBBtxCD......",
"..EuFGAg........",
"..HIJKL.........",
"..MNOz..........",
"...Pz...........",
"...Q............",
"................",
"................"};

static const char* const image13_data[] = { 
"16 16 79 2",
".8 c None",
".3 c None",
".S c None",
".q c None",
".z c None",
".h c None",
".f c None",
"#m c None",
".g c None",
".e c None",
".d c None",
".n c None",
".U c None",
".m c None",
".c c None",
".Z c None",
".b c None",
".l c None",
".T c None",
".a c None",
".i c None",
".# c None",
"Qt c None",
"#k c #bd0000",
"#l c #c50000",
"#g c #cd0000",
".y c #cd1000",
".H c #cd1400",
"#h c #cd1800",
"#e c #d50000",
"#a c #d50400",
".7 c #d50c00",
".p c #d51800",
".o c #d52000",
"#f c #d52400",
".4 c #d52800",
".E c #de2808",
".v c #de2c08",
".R c #de3008",
".j c #e63c00",
".M c #e63c10",
".D c #e64410",
".r c #ee3c08",
"#i c #ee4810",
".k c #ee4c10",
".I c #ee4c18",
".u c #ee5018",
"#j c #f60000",
".x c #f64810",
"#d c #ff0000",
"#c c #ff0800",
"#b c #ff1000",
"## c #ff1400",
"#. c #ff2408",
".6 c #ff2800",
".9 c #ff2c08",
".5 c #ff3c10",
".2 c #ff4410",
".G c #ff5018",
".1 c #ff5520",
".F c #ff6120",
".N c #ff6129",
".w c #ff6529",
".Y c #ff6929",
".0 c #ff6d31",
".X c #ff7131",
".Q c #ff8139",
".V c #ff8141",
".W c #ff854a",
".O c #ff9152",
".L c #ff9552",
".P c #ff9962",
".s c #ff9d62",
".C c #ffa162",
".J c #ffa562",
".K c #ffae7b",
".A c #ffb67b",
".t c #ffba7b",
".B c #ffc294",
"Qt.#Qt.a.a.b.c.d.d.e.d.e.e.f.g.h",
".#.i.j.k.b.l.m.n.e.d.e.e.o.p.q.h",
"Qt.r.s.t.u.m.c.d.d.e.e.v.w.x.y.z",
".a.k.A.B.C.D.d.m.e.g.E.w.F.G.H.z",
"Qt.b.I.J.K.L.M.e.e.E.w.N.G.p.z.h",
".i.a.m.D.O.P.Q.R.E.w.F.G.p.z.S.h",
"Qt.T.b.U.M.V.W.X.Y.N.G.p.z.z.h.h",
".i.a.Z.m.e.R.X.0.F.G.p.z.S.h.S.h",
".d.e.d.e.e.v.Y.F.1.2.p.S.z.3.3.3",
".e.d.e.g.4.w.F.G.2.5.6.7.8.3.8.3",
".d.e.e.v.w.N.G.p.p.9#.###a.3.3.3",
".e.e.E.w.F.G.p.z.3.7#b#c#d#e.8.3",
".e#f.w.w.G.p.z.z.z.3#a#d#d#d#g.8",
".f#h#i.G.p.z.S.h.S.S.8#e#d#j#k.3",
".e.h.y.H.z.z.h.z.z.3.S.3#g#l.3.3",
"#m.f.S.z.S.h.S.h.S.S.3.3.8.3.3.3"};


/* 
 *  Constructs a PreferencesDialog as a child of 'parent', with the 
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  TRUE to construct a modal dialog.
 */
PreferencesDialog::PreferencesDialog( QWidget* parent, const char* name, bool modal, WFlags fl )
    : QDialog( parent, name, modal, fl )
,
      image0( (const char **) image0_data ),
      image1( (const char **) image1_data ),
      image2( (const char **) image2_data ),
      image3( (const char **) image3_data ),
      image4( (const char **) image4_data ),
      image5( (const char **) image5_data ),
      image6( (const char **) image6_data ),
      image7( (const char **) image7_data ),
      image8( (const char **) image8_data ),
      image9( (const char **) image9_data ),
      image10( (const char **) image10_data ),
      image11( (const char **) image11_data ),
      image12( (const char **) image12_data ),
      image13( (const char **) image13_data )
{
    if ( !name )
	setName( "PreferencesDialog" );
    setSizeGripEnabled( TRUE );
    PreferencesDialogLayout = new QGridLayout( this, 1, 1, 11, 6, "PreferencesDialogLayout"); 

    layout70 = new QVBoxLayout( 0, 0, 6, "layout70"); 

    tabWidget = new QTabWidget( this, "tabWidget" );

    tab = new QWidget( tabWidget, "tab" );
    tabLayout = new QGridLayout( tab, 1, 1, 11, 6, "tabLayout"); 

    layout53 = new QVBoxLayout( 0, 0, 6, "layout53"); 

    layout45 = new QHBoxLayout( 0, 0, 6, "layout45"); 

    TextLabel_Warn2 = new QLabel( tab, "TextLabel_Warn2" );
    TextLabel_Warn2->setPaletteForegroundColor( QColor( 170, 0, 0 ) );
    QFont TextLabel_Warn2_font(  TextLabel_Warn2->font() );
    TextLabel_Warn2_font.setBold( TRUE );
    TextLabel_Warn2->setFont( TextLabel_Warn2_font ); 
    TextLabel_Warn2->setTextFormat( QLabel::AutoText );
    TextLabel_Warn2->setAlignment( int( QLabel::AlignCenter ) );
    layout45->addWidget( TextLabel_Warn2 );

    pixmapLabel1 = new QLabel( tab, "pixmapLabel1" );
    pixmapLabel1->setPixmap( image0 );
    pixmapLabel1->setScaledContents( FALSE );
    layout45->addWidget( pixmapLabel1 );
    layout53->addLayout( layout45 );
    QSpacerItem* spacer = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout53->addItem( spacer );

    layout52 = new QVBoxLayout( 0, 0, 6, "layout52"); 

    TextLabel1_3_5 = new QLabel( tab, "TextLabel1_3_5" );
    QFont TextLabel1_3_5_font(  TextLabel1_3_5->font() );
    TextLabel1_3_5_font.setBold( TRUE );
    TextLabel1_3_5->setFont( TextLabel1_3_5_font ); 
    TextLabel1_3_5->setTextFormat( QLabel::AutoText );
    layout52->addWidget( TextLabel1_3_5 );

    Line1_2_5 = new QFrame( tab, "Line1_2_5" );
    Line1_2_5->setFrameShape( QFrame::HLine );
    Line1_2_5->setFrameShadow( QFrame::Sunken );
    Line1_2_5->setFrameShape( QFrame::HLine );
    layout52->addWidget( Line1_2_5 );

    layout46 = new QHBoxLayout( 0, 0, 6, "layout46"); 

    LineEdit_LogFile = new QLineEdit( tab, "LineEdit_LogFile" );
    layout46->addWidget( LineEdit_LogFile );

    ToolButton_BrowseLog = new QToolButton( tab, "ToolButton_BrowseLog" );
    ToolButton_BrowseLog->setIconSet( QIconSet( image1 ) );
    layout46->addWidget( ToolButton_BrowseLog );
    layout52->addLayout( layout46 );
    layout53->addLayout( layout52 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout53->addItem( spacer_2 );

    layout51 = new QVBoxLayout( 0, 0, 6, "layout51"); 

    TextLabel1_3_5_2 = new QLabel( tab, "TextLabel1_3_5_2" );
    QFont TextLabel1_3_5_2_font(  TextLabel1_3_5_2->font() );
    TextLabel1_3_5_2_font.setBold( TRUE );
    TextLabel1_3_5_2->setFont( TextLabel1_3_5_2_font ); 
    TextLabel1_3_5_2->setTextFormat( QLabel::AutoText );
    layout51->addWidget( TextLabel1_3_5_2 );

    Line1_2_5_2 = new QFrame( tab, "Line1_2_5_2" );
    Line1_2_5_2->setFrameShape( QFrame::HLine );
    Line1_2_5_2->setFrameShadow( QFrame::Sunken );
    Line1_2_5_2->setFrameShape( QFrame::HLine );
    layout51->addWidget( Line1_2_5_2 );

    layout47 = new QHBoxLayout( 0, 0, 6, "layout47"); 

    LineEdit_ADF = new QLineEdit( tab, "LineEdit_ADF" );
    layout47->addWidget( LineEdit_ADF );

    ToolButton_BrowseADF = new QToolButton( tab, "ToolButton_BrowseADF" );
    ToolButton_BrowseADF->setIconSet( QIconSet( image1 ) );
    layout47->addWidget( ToolButton_BrowseADF );
    layout51->addLayout( layout47 );
    layout53->addLayout( layout51 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout53->addItem( spacer_3 );

    layout50 = new QVBoxLayout( 0, 0, 6, "layout50"); 

    TextLabel1_3_5_2_2 = new QLabel( tab, "TextLabel1_3_5_2_2" );
    QFont TextLabel1_3_5_2_2_font(  TextLabel1_3_5_2_2->font() );
    TextLabel1_3_5_2_2_font.setBold( TRUE );
    TextLabel1_3_5_2_2->setFont( TextLabel1_3_5_2_2_font ); 
    TextLabel1_3_5_2_2->setTextFormat( QLabel::AutoText );
    layout50->addWidget( TextLabel1_3_5_2_2 );

    Line1_2_5_2_2 = new QFrame( tab, "Line1_2_5_2_2" );
    Line1_2_5_2_2->setFrameShape( QFrame::HLine );
    Line1_2_5_2_2->setFrameShadow( QFrame::Sunken );
    Line1_2_5_2_2->setFrameShape( QFrame::HLine );
    layout50->addWidget( Line1_2_5_2_2 );

    CheckBox_ValidateADF = new QCheckBox( tab, "CheckBox_ValidateADF" );
    CheckBox_ValidateADF->setChecked( FALSE );
    layout50->addWidget( CheckBox_ValidateADF );
    layout53->addLayout( layout50 );
    QSpacerItem* spacer_4 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout53->addItem( spacer_4 );

    layout49 = new QVBoxLayout( 0, 0, 6, "layout49"); 

    TextLabel5 = new QLabel( tab, "TextLabel5" );
    layout49->addWidget( TextLabel5 );

    layout48 = new QHBoxLayout( 0, 0, 6, "layout48"); 

    LineEdit_ValidationCmd = new QLineEdit( tab, "LineEdit_ValidationCmd" );
    LineEdit_ValidationCmd->setEnabled( FALSE );
    layout48->addWidget( LineEdit_ValidationCmd );

    ToolButton_BrowseCmd = new QToolButton( tab, "ToolButton_BrowseCmd" );
    ToolButton_BrowseCmd->setIconSet( QIconSet( image1 ) );
    layout48->addWidget( ToolButton_BrowseCmd );
    layout49->addLayout( layout48 );
    layout53->addLayout( layout49 );
    QSpacerItem* spacer_5 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout53->addItem( spacer_5 );

    tabLayout->addLayout( layout53, 0, 0 );
    tabWidget->insertTab( tab, "" );

    Widget8 = new QWidget( tabWidget, "Widget8" );
    Widget8Layout = new QGridLayout( Widget8, 1, 1, 11, 6, "Widget8Layout"); 

    layout54 = new QVBoxLayout( 0, 0, 6, "layout54"); 

    layout44 = new QVBoxLayout( 0, 0, 6, "layout44"); 

    pixmapLabel1_2 = new QLabel( Widget8, "pixmapLabel1_2" );
    pixmapLabel1_2->setPixmap( image2 );
    pixmapLabel1_2->setScaledContents( FALSE );
    pixmapLabel1_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout44->addWidget( pixmapLabel1_2 );
    QSpacerItem* spacer_6 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout44->addItem( spacer_6 );

    layout42 = new QVBoxLayout( 0, 0, 6, "layout42"); 

    TextLabel1 = new QLabel( Widget8, "TextLabel1" );
    QFont TextLabel1_font(  TextLabel1->font() );
    TextLabel1_font.setBold( TRUE );
    TextLabel1->setFont( TextLabel1_font ); 
    TextLabel1->setTextFormat( QLabel::AutoText );
    layout42->addWidget( TextLabel1 );

    Line1 = new QFrame( Widget8, "Line1" );
    Line1->setFrameShape( QFrame::HLine );
    Line1->setFrameShadow( QFrame::Sunken );
    Line1->setFrameShape( QFrame::HLine );
    layout42->addWidget( Line1 );

    layout40 = new QHBoxLayout( 0, 0, 6, "layout40"); 

    LineEdit_Relationship = new QLineEdit( Widget8, "LineEdit_Relationship" );
    layout40->addWidget( LineEdit_Relationship );

    ToolButton_AvailableTags = new QToolButton( Widget8, "ToolButton_AvailableTags" );
    ToolButton_AvailableTags->setIconSet( QIconSet( image3 ) );
    layout40->addWidget( ToolButton_AvailableTags );
    layout42->addLayout( layout40 );

    TextLabel1_2_2 = new QLabel( Widget8, "TextLabel1_2_2" );
    QFont TextLabel1_2_2_font(  TextLabel1_2_2->font() );
    TextLabel1_2_2_font.setBold( TRUE );
    TextLabel1_2_2->setFont( TextLabel1_2_2_font ); 
    TextLabel1_2_2->setTextFormat( QLabel::AutoText );
    layout42->addWidget( TextLabel1_2_2 );

    Line2_2 = new QFrame( Widget8, "Line2_2" );
    Line2_2->setFrameShape( QFrame::HLine );
    Line2_2->setFrameShadow( QFrame::Sunken );
    Line2_2->setFrameShape( QFrame::HLine );
    layout42->addWidget( Line2_2 );

    layout39 = new QHBoxLayout( 0, 0, 6, "layout39"); 

    ListBox_Favorite = new QListBox( Widget8, "ListBox_Favorite" );
    layout39->addWidget( ListBox_Favorite );

    layout36 = new QVBoxLayout( 0, 0, 6, "layout36"); 

    ToolButton_FavoriteUp = new QToolButton( Widget8, "ToolButton_FavoriteUp" );
    ToolButton_FavoriteUp->setIconSet( QIconSet( image4 ) );
    layout36->addWidget( ToolButton_FavoriteUp );
    QSpacerItem* spacer_7 = new QSpacerItem( 20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout36->addItem( spacer_7 );

    ToolButton_FavoriteDown = new QToolButton( Widget8, "ToolButton_FavoriteDown" );
    ToolButton_FavoriteDown->setIconSet( QIconSet( image5 ) );
    layout36->addWidget( ToolButton_FavoriteDown );
    layout39->addLayout( layout36 );

    layout35 = new QVBoxLayout( 0, 0, 6, "layout35"); 

    ToolButton_AddFavorite = new QToolButton( Widget8, "ToolButton_AddFavorite" );
    ToolButton_AddFavorite->setIconSet( QIconSet( image6 ) );
    layout35->addWidget( ToolButton_AddFavorite );
    QSpacerItem* spacer_8 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout35->addItem( spacer_8 );

    ToolButton_RemoveFavorite = new QToolButton( Widget8, "ToolButton_RemoveFavorite" );
    ToolButton_RemoveFavorite->setIconSet( QIconSet( image7 ) );
    layout35->addWidget( ToolButton_RemoveFavorite );
    layout39->addLayout( layout35 );
    layout42->addLayout( layout39 );
    layout44->addLayout( layout42 );
    layout54->addLayout( layout44 );
    QSpacerItem* spacer_9 = new QSpacerItem( 20, 130, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout54->addItem( spacer_9 );

    Widget8Layout->addLayout( layout54, 0, 0 );
    tabWidget->insertTab( Widget8, "" );

    tab_2 = new QWidget( tabWidget, "tab_2" );
    tabLayout_2 = new QGridLayout( tab_2, 1, 1, 11, 6, "tabLayout_2"); 

    layout69 = new QVBoxLayout( 0, 0, 6, "layout69"); 

    pixmapLabel1_3 = new QLabel( tab_2, "pixmapLabel1_3" );
    pixmapLabel1_3->setPixmap( image8 );
    pixmapLabel1_3->setScaledContents( FALSE );
    pixmapLabel1_3->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout69->addWidget( pixmapLabel1_3 );

    layout55 = new QVBoxLayout( 0, 0, 6, "layout55"); 

    TextLabel1_4 = new QLabel( tab_2, "TextLabel1_4" );
    QFont TextLabel1_4_font(  TextLabel1_4->font() );
    TextLabel1_4_font.setBold( TRUE );
    TextLabel1_4->setFont( TextLabel1_4_font ); 
    TextLabel1_4->setTextFormat( QLabel::AutoText );
    layout55->addWidget( TextLabel1_4 );

    Line1_3 = new QFrame( tab_2, "Line1_3" );
    Line1_3->setFrameShape( QFrame::HLine );
    Line1_3->setFrameShadow( QFrame::Sunken );
    Line1_3->setFrameShape( QFrame::HLine );
    layout55->addWidget( Line1_3 );
    layout69->addLayout( layout55 );

    layout59 = new QHBoxLayout( 0, 0, 6, "layout59"); 

    TextLabel6 = new QLabel( tab_2, "TextLabel6" );
    layout59->addWidget( TextLabel6 );
    QSpacerItem* spacer_10 = new QSpacerItem( 110, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout59->addItem( spacer_10 );

    PixmapLabel_BgColor = new QLabel( tab_2, "PixmapLabel_BgColor" );
    PixmapLabel_BgColor->setMinimumSize( QSize( 50, 24 ) );
    PixmapLabel_BgColor->setPaletteBackgroundColor( QColor( 255, 170, 0 ) );
    PixmapLabel_BgColor->setFrameShape( QLabel::Box );
    PixmapLabel_BgColor->setFrameShadow( QLabel::Raised );
    PixmapLabel_BgColor->setScaledContents( TRUE );
    layout59->addWidget( PixmapLabel_BgColor );

    ToolButton_BgColor = new QToolButton( tab_2, "ToolButton_BgColor" );
    ToolButton_BgColor->setMinimumSize( QSize( 24, 24 ) );
    ToolButton_BgColor->setIconSet( QIconSet( image9 ) );
    layout59->addWidget( ToolButton_BgColor );
    layout69->addLayout( layout59 );

    layout60 = new QHBoxLayout( 0, 0, 6, "layout60"); 

    TextLabel6_2 = new QLabel( tab_2, "TextLabel6_2" );
    layout60->addWidget( TextLabel6_2 );
    QSpacerItem* spacer_11 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout60->addItem( spacer_11 );

    PixmapLabel_ShadingColor = new QLabel( tab_2, "PixmapLabel_ShadingColor" );
    PixmapLabel_ShadingColor->setMinimumSize( QSize( 50, 24 ) );
    PixmapLabel_ShadingColor->setPaletteBackgroundColor( QColor( 255, 170, 0 ) );
    PixmapLabel_ShadingColor->setFrameShape( QLabel::Box );
    PixmapLabel_ShadingColor->setFrameShadow( QLabel::Raised );
    PixmapLabel_ShadingColor->setScaledContents( TRUE );
    layout60->addWidget( PixmapLabel_ShadingColor );

    ToolButton_ShadingColor = new QToolButton( tab_2, "ToolButton_ShadingColor" );
    ToolButton_ShadingColor->setMinimumSize( QSize( 24, 24 ) );
    ToolButton_ShadingColor->setIconSet( QIconSet( image9 ) );
    layout60->addWidget( ToolButton_ShadingColor );
    layout69->addLayout( layout60 );

    layout62 = new QHBoxLayout( 0, 0, 6, "layout62"); 

    TextLabel6_2_2 = new QLabel( tab_2, "TextLabel6_2_2" );
    layout62->addWidget( TextLabel6_2_2 );

    ComboBox_GUIStyle = new QComboBox( FALSE, tab_2, "ComboBox_GUIStyle" );
    layout62->addWidget( ComboBox_GUIStyle );
    layout69->addLayout( layout62 );

    layout63 = new QHBoxLayout( 0, 0, 6, "layout63"); 

    TextLabel6_2_2_2 = new QLabel( tab_2, "TextLabel6_2_2_2" );
    layout63->addWidget( TextLabel6_2_2_2 );

    SpinBox_FontSize = new QSpinBox( tab_2, "SpinBox_FontSize" );
    SpinBox_FontSize->setMaxValue( 24 );
    SpinBox_FontSize->setMinValue( 8 );
    SpinBox_FontSize->setValue( 12 );
    layout63->addWidget( SpinBox_FontSize );
    layout69->addLayout( layout63 );
    QSpacerItem* spacer_12 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout69->addItem( spacer_12 );

    layout68 = new QVBoxLayout( 0, 0, 6, "layout68"); 

    TextLabel1_4_2 = new QLabel( tab_2, "TextLabel1_4_2" );
    QFont TextLabel1_4_2_font(  TextLabel1_4_2->font() );
    TextLabel1_4_2_font.setBold( TRUE );
    TextLabel1_4_2->setFont( TextLabel1_4_2_font ); 
    TextLabel1_4_2->setTextFormat( QLabel::AutoText );
    layout68->addWidget( TextLabel1_4_2 );

    Line1_3_2 = new QFrame( tab_2, "Line1_3_2" );
    Line1_3_2->setFrameShape( QFrame::HLine );
    Line1_3_2->setFrameShadow( QFrame::Sunken );
    Line1_3_2->setFrameShape( QFrame::HLine );
    layout68->addWidget( Line1_3_2 );

    layout67 = new QHBoxLayout( 0, 0, 6, "layout67"); 

    layout66 = new QVBoxLayout( 0, 0, 6, "layout66"); 

    CheckBox_SnapGrid = new QCheckBox( tab_2, "CheckBox_SnapGrid" );
    CheckBox_SnapGrid->setEnabled( FALSE );
    layout66->addWidget( CheckBox_SnapGrid );

    CheckBox_ShowHGrid = new QCheckBox( tab_2, "CheckBox_ShowHGrid" );
    layout66->addWidget( CheckBox_ShowHGrid );

    CheckBox_ShowVGrid = new QCheckBox( tab_2, "CheckBox_ShowVGrid" );
    layout66->addWidget( CheckBox_ShowVGrid );
    layout67->addLayout( layout66 );
    QSpacerItem* spacer_13 = new QSpacerItem( 40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout67->addItem( spacer_13 );

    layout65 = new QVBoxLayout( 0, 0, 6, "layout65"); 

    TextLabel6_2_2_2_2 = new QLabel( tab_2, "TextLabel6_2_2_2_2" );
    TextLabel6_2_2_2_2->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout65->addWidget( TextLabel6_2_2_2_2 );

    LineEdit_GridSize = new QLineEdit( tab_2, "LineEdit_GridSize" );
    LineEdit_GridSize->setReadOnly( FALSE );
    layout65->addWidget( LineEdit_GridSize );

    Slider_GridSize = new QSlider( tab_2, "Slider_GridSize" );
    Slider_GridSize->setMinValue( 1 );
    Slider_GridSize->setMaxValue( 100 );
    Slider_GridSize->setLineStep( 1 );
    Slider_GridSize->setPageStep( 10 );
    Slider_GridSize->setValue( 1 );
    Slider_GridSize->setOrientation( QSlider::Horizontal );
    Slider_GridSize->setTickmarks( QSlider::NoMarks );
    Slider_GridSize->setTickInterval( 10 );
    layout65->addWidget( Slider_GridSize );
    layout67->addLayout( layout65 );
    layout68->addLayout( layout67 );
    layout69->addLayout( layout68 );

    tabLayout_2->addLayout( layout69, 0, 0 );
    tabWidget->insertTab( tab_2, "" );

    tab_3 = new QWidget( tabWidget, "tab_3" );
    tabLayout_3 = new QGridLayout( tab_3, 1, 1, 11, 6, "tabLayout_3"); 

    layout81 = new QVBoxLayout( 0, 0, 6, "layout81"); 

    pixmapLabel1_4 = new QLabel( tab_3, "pixmapLabel1_4" );
    pixmapLabel1_4->setPixmap( image10 );
    pixmapLabel1_4->setScaledContents( FALSE );
    pixmapLabel1_4->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout81->addWidget( pixmapLabel1_4 );
    QSpacerItem* spacer_14 = new QSpacerItem( 20, 100, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout81->addItem( spacer_14 );

    layout71 = new QVBoxLayout( 0, 0, 6, "layout71"); 

    TextLabel1_4_2_2_2_2 = new QLabel( tab_3, "TextLabel1_4_2_2_2_2" );
    QFont TextLabel1_4_2_2_2_2_font(  TextLabel1_4_2_2_2_2->font() );
    TextLabel1_4_2_2_2_2_font.setBold( TRUE );
    TextLabel1_4_2_2_2_2->setFont( TextLabel1_4_2_2_2_2_font ); 
    TextLabel1_4_2_2_2_2->setTextFormat( QLabel::AutoText );
    layout71->addWidget( TextLabel1_4_2_2_2_2 );

    Line1_3_2_2_2_2 = new QFrame( tab_3, "Line1_3_2_2_2_2" );
    Line1_3_2_2_2_2->setFrameShape( QFrame::HLine );
    Line1_3_2_2_2_2->setFrameShadow( QFrame::Sunken );
    Line1_3_2_2_2_2->setFrameShape( QFrame::HLine );
    layout71->addWidget( Line1_3_2_2_2_2 );

    CheckBox_TagWinFavouriteOnly = new QCheckBox( tab_3, "CheckBox_TagWinFavouriteOnly" );
    layout71->addWidget( CheckBox_TagWinFavouriteOnly );
    layout81->addLayout( layout71 );
    QSpacerItem* spacer_15 = new QSpacerItem( 20, 70, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout81->addItem( spacer_15 );

    layout72 = new QVBoxLayout( 0, 0, 6, "layout72"); 

    TextLabel1_4_2_2_2_2_2 = new QLabel( tab_3, "TextLabel1_4_2_2_2_2_2" );
    QFont TextLabel1_4_2_2_2_2_2_font(  TextLabel1_4_2_2_2_2_2->font() );
    TextLabel1_4_2_2_2_2_2_font.setBold( TRUE );
    TextLabel1_4_2_2_2_2_2->setFont( TextLabel1_4_2_2_2_2_2_font ); 
    TextLabel1_4_2_2_2_2_2->setTextFormat( QLabel::AutoText );
    layout72->addWidget( TextLabel1_4_2_2_2_2_2 );

    Line1_3_2_2_2_2_2 = new QFrame( tab_3, "Line1_3_2_2_2_2_2" );
    Line1_3_2_2_2_2_2->setFrameShape( QFrame::HLine );
    Line1_3_2_2_2_2_2->setFrameShadow( QFrame::Sunken );
    Line1_3_2_2_2_2_2->setFrameShape( QFrame::HLine );
    layout72->addWidget( Line1_3_2_2_2_2_2 );

    CheckBox_SynchTabs = new QCheckBox( tab_3, "CheckBox_SynchTabs" );
    layout72->addWidget( CheckBox_SynchTabs );
    layout81->addLayout( layout72 );
    QSpacerItem* spacer_16 = new QSpacerItem( 20, 60, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout81->addItem( spacer_16 );

    tabLayout_3->addLayout( layout81, 0, 0 );
    tabWidget->insertTab( tab_3, "" );

    tab_4 = new QWidget( tabWidget, "tab_4" );
    tabLayout_4 = new QGridLayout( tab_4, 1, 1, 11, 6, "tabLayout_4"); 

    layout80 = new QVBoxLayout( 0, 0, 6, "layout80"); 

    pixmapLabel1_5 = new QLabel( tab_4, "pixmapLabel1_5" );
    pixmapLabel1_5->setPixmap( image11 );
    pixmapLabel1_5->setScaledContents( FALSE );
    pixmapLabel1_5->setAlignment( int( QLabel::AlignVCenter | QLabel::AlignRight ) );
    layout80->addWidget( pixmapLabel1_5 );
    QSpacerItem* spacer_17 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout80->addItem( spacer_17 );

    layout75 = new QVBoxLayout( 0, 0, 6, "layout75"); 

    TextLabel1_4_2_2_2_2_2_2 = new QLabel( tab_4, "TextLabel1_4_2_2_2_2_2_2" );
    QFont TextLabel1_4_2_2_2_2_2_2_font(  TextLabel1_4_2_2_2_2_2_2->font() );
    TextLabel1_4_2_2_2_2_2_2_font.setBold( TRUE );
    TextLabel1_4_2_2_2_2_2_2->setFont( TextLabel1_4_2_2_2_2_2_2_font ); 
    TextLabel1_4_2_2_2_2_2_2->setTextFormat( QLabel::AutoText );
    layout75->addWidget( TextLabel1_4_2_2_2_2_2_2 );

    Line1_3_2_2_2_2_2_2 = new QFrame( tab_4, "Line1_3_2_2_2_2_2_2" );
    Line1_3_2_2_2_2_2_2->setFrameShape( QFrame::HLine );
    Line1_3_2_2_2_2_2_2->setFrameShadow( QFrame::Sunken );
    Line1_3_2_2_2_2_2_2->setFrameShape( QFrame::HLine );
    layout75->addWidget( Line1_3_2_2_2_2_2_2 );

    CheckBox_RelaxedEENode = new QCheckBox( tab_4, "CheckBox_RelaxedEENode" );
    CheckBox_RelaxedEENode->setChecked( TRUE );
    layout75->addWidget( CheckBox_RelaxedEENode );

    CheckBox_BackPropagate = new QCheckBox( tab_4, "CheckBox_BackPropagate" );
    CheckBox_BackPropagate->setChecked( TRUE );
    layout75->addWidget( CheckBox_BackPropagate );

    CheckBox_CompressMutable = new QCheckBox( tab_4, "CheckBox_CompressMutable" );
    CheckBox_CompressMutable->setChecked( TRUE );
    layout75->addWidget( CheckBox_CompressMutable );
    layout80->addLayout( layout75 );
    QSpacerItem* spacer_18 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout80->addItem( spacer_18 );

    layout79 = new QVBoxLayout( 0, 0, 6, "layout79"); 

    TextLabel1_4_2_2_2 = new QLabel( tab_4, "TextLabel1_4_2_2_2" );
    QFont TextLabel1_4_2_2_2_font(  TextLabel1_4_2_2_2->font() );
    TextLabel1_4_2_2_2_font.setBold( TRUE );
    TextLabel1_4_2_2_2->setFont( TextLabel1_4_2_2_2_font ); 
    TextLabel1_4_2_2_2->setTextFormat( QLabel::AutoText );
    layout79->addWidget( TextLabel1_4_2_2_2 );

    Line1_3_2_2_2 = new QFrame( tab_4, "Line1_3_2_2_2" );
    Line1_3_2_2_2->setFrameShape( QFrame::HLine );
    Line1_3_2_2_2->setFrameShadow( QFrame::Sunken );
    Line1_3_2_2_2->setFrameShape( QFrame::HLine );
    layout79->addWidget( Line1_3_2_2_2 );

    CheckBox_AutoPurge = new QCheckBox( tab_4, "CheckBox_AutoPurge" );
    layout79->addWidget( CheckBox_AutoPurge );

    layout76 = new QHBoxLayout( 0, 0, 6, "layout76"); 

    CheckBox_IncPurge = new QCheckBox( tab_4, "CheckBox_IncPurge" );
    QFont CheckBox_IncPurge_font(  CheckBox_IncPurge->font() );
    CheckBox_IncPurge->setFont( CheckBox_IncPurge_font ); 
    layout76->addWidget( CheckBox_IncPurge );

    SpinBox_MaxAge = new QSpinBox( tab_4, "SpinBox_MaxAge" );
    SpinBox_MaxAge->setEnabled( FALSE );
    SpinBox_MaxAge->setMaxValue( 9999999 );
    SpinBox_MaxAge->setMinValue( 10 );
    SpinBox_MaxAge->setLineStep( 100 );
    SpinBox_MaxAge->setValue( 1000 );
    layout76->addWidget( SpinBox_MaxAge );
    layout79->addLayout( layout76 );

    CheckBox_MaxPurge = new QCheckBox( tab_4, "CheckBox_MaxPurge" );
    layout79->addWidget( CheckBox_MaxPurge );

    layout78 = new QHBoxLayout( 0, 0, 6, "layout78"); 

    Slider_MaxInflight = new QSlider( tab_4, "Slider_MaxInflight" );
    Slider_MaxInflight->setEnabled( FALSE );
    Slider_MaxInflight->setMinValue( 1000 );
    Slider_MaxInflight->setMaxValue( 10000000 );
    Slider_MaxInflight->setLineStep( 1000 );
    Slider_MaxInflight->setPageStep( 10000 );
    Slider_MaxInflight->setValue( 1000 );
    Slider_MaxInflight->setOrientation( QSlider::Horizontal );
    Slider_MaxInflight->setTickmarks( QSlider::NoMarks );
    Slider_MaxInflight->setTickInterval( 1000 );
    layout78->addWidget( Slider_MaxInflight );
    QSpacerItem* spacer_19 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    layout78->addItem( spacer_19 );

    LineEdit_MaxInflight = new QLineEdit( tab_4, "LineEdit_MaxInflight" );
    LineEdit_MaxInflight->setEnabled( FALSE );
    LineEdit_MaxInflight->setReadOnly( FALSE );
    layout78->addWidget( LineEdit_MaxInflight );
    layout79->addLayout( layout78 );
    layout80->addLayout( layout79 );
    QSpacerItem* spacer_20 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout80->addItem( spacer_20 );

    tabLayout_4->addLayout( layout80, 0, 0 );
    tabWidget->insertTab( tab_4, "" );
    layout70->addWidget( tabWidget );
    QSpacerItem* spacer_21 = new QSpacerItem( 20, 16, QSizePolicy::Minimum, QSizePolicy::Expanding );
    layout70->addItem( spacer_21 );

    Layout1 = new QHBoxLayout( 0, 0, 6, "Layout1"); 
    QSpacerItem* spacer_22 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    Layout1->addItem( spacer_22 );

    buttonOk = new QPushButton( this, "buttonOk" );
    buttonOk->setMinimumSize( QSize( 75, 0 ) );
    buttonOk->setPixmap( image12 );
    buttonOk->setAutoDefault( TRUE );
    buttonOk->setDefault( TRUE );
    Layout1->addWidget( buttonOk );

    buttonCancel = new QPushButton( this, "buttonCancel" );
    buttonCancel->setMinimumSize( QSize( 75, 0 ) );
    buttonCancel->setPixmap( image13 );
    buttonCancel->setAutoDefault( TRUE );
    Layout1->addWidget( buttonCancel );
    layout70->addLayout( Layout1 );

    PreferencesDialogLayout->addLayout( layout70, 0, 0 );
    languageChange();
    resize( QSize(409, 542).expandedTo(minimumSizeHint()) );

    // signals and slots connections
    connect( buttonOk, SIGNAL( clicked() ), this, SLOT( accept() ) );
    connect( buttonCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );

    // tab order
    setTabOrder( tabWidget, LineEdit_LogFile );
    setTabOrder( LineEdit_LogFile, LineEdit_ADF );
    setTabOrder( LineEdit_ADF, CheckBox_ValidateADF );
    setTabOrder( CheckBox_ValidateADF, LineEdit_ValidationCmd );
    setTabOrder( LineEdit_ValidationCmd, LineEdit_Relationship );
    setTabOrder( LineEdit_Relationship, ListBox_Favorite );
    setTabOrder( ListBox_Favorite, ComboBox_GUIStyle );
    setTabOrder( ComboBox_GUIStyle, SpinBox_FontSize );
    setTabOrder( SpinBox_FontSize, CheckBox_SnapGrid );
    setTabOrder( CheckBox_SnapGrid, CheckBox_ShowHGrid );
    setTabOrder( CheckBox_ShowHGrid, Slider_GridSize );
    setTabOrder( Slider_GridSize, LineEdit_GridSize );
    setTabOrder( LineEdit_GridSize, CheckBox_TagWinFavouriteOnly );
    setTabOrder( CheckBox_TagWinFavouriteOnly, CheckBox_SynchTabs );
    setTabOrder( CheckBox_SynchTabs, CheckBox_RelaxedEENode );
    setTabOrder( CheckBox_RelaxedEENode, CheckBox_BackPropagate );
    setTabOrder( CheckBox_BackPropagate, CheckBox_CompressMutable );
    setTabOrder( CheckBox_CompressMutable, CheckBox_AutoPurge );
    setTabOrder( CheckBox_AutoPurge, CheckBox_IncPurge );
    setTabOrder( CheckBox_IncPurge, SpinBox_MaxAge );
    setTabOrder( SpinBox_MaxAge, CheckBox_MaxPurge );
    setTabOrder( CheckBox_MaxPurge, Slider_MaxInflight );
    setTabOrder( Slider_MaxInflight, LineEdit_MaxInflight );
    setTabOrder( LineEdit_MaxInflight, buttonOk );
    setTabOrder( buttonOk, buttonCancel );
}

/*
 *  Destroys the object and frees any allocated resources
 */
PreferencesDialog::~PreferencesDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PreferencesDialog::languageChange()
{
    setCaption( tr( "2Dreams Preferences" ) );
    TextLabel_Warn2->setText( tr( "Warning, close file before changing these settings!" ) );
    TextLabel1_3_5->setText( tr( "Log information file:" ) );
    QToolTip::add( LineEdit_LogFile, tr( "Log information file path" ) );
    ToolButton_BrowseLog->setText( QString::null );
    TextLabel1_3_5_2->setText( tr( "Architecture Definition File (ADF):" ) );
    QToolTip::add( LineEdit_ADF, tr( "Specify the ADF used by 2Dreams to know how to scatter  information on the screen." ) );
    ToolButton_BrowseADF->setText( QString::null );
    TextLabel1_3_5_2_2->setText( tr( "ADF Validation:" ) );
    CheckBox_ValidateADF->setText( tr( "Perform Validation" ) );
    QToolTip::add( CheckBox_ValidateADF, tr( "If set, 2Dreams will perform a DTD validation of the ADF." ) );
    TextLabel5->setText( tr( "Command line:" ) );
    QToolTip::add( LineEdit_ValidationCmd, tr( "Command line for DTD validation" ) );
    ToolButton_BrowseCmd->setText( QString::null );
    tabWidget->changeTab( tab, tr( "Files" ) );
    TextLabel1->setText( tr( "Relationship tracking TAG:" ) );
    QToolTip::add( LineEdit_Relationship, tr( "Tag used on relationship tracking options." ) );
    ToolButton_AvailableTags->setText( QString::null );
    TextLabel1_2_2->setText( tr( "Favourite TAG List:" ) );
    QToolTip::add( ListBox_Favorite, tr( "List of \"Favorite\" Tags used for Highlighting and Tag Window." ) );
    ToolButton_FavoriteUp->setText( QString::null );
    ToolButton_FavoriteDown->setText( QString::null );
    ToolButton_AddFavorite->setText( QString::null );
    ToolButton_RemoveFavorite->setText( QString::null );
    tabWidget->changeTab( Widget8, tr( "TAGS" ) );
    TextLabel1_4->setText( tr( "Appearence Settings:" ) );
    TextLabel6->setText( tr( "Event Background Color:" ) );
    ToolButton_BgColor->setText( QString::null );
    TextLabel6_2->setText( tr( "Column Shading Color:" ) );
    ToolButton_ShadingColor->setText( QString::null );
    TextLabel6_2_2->setText( tr( "GUI Style:" ) );
    TextLabel6_2_2_2->setText( tr( "Font Size:" ) );
    TextLabel1_4_2->setText( tr( "Grid Lines:" ) );
    CheckBox_SnapGrid->setText( tr( "Snap to Grid" ) );
    QToolTip::add( CheckBox_SnapGrid, tr( "Snap to grid when adding annotations (disabled by now)." ) );
    CheckBox_ShowHGrid->setText( tr( "Show Horizontal Grid Lines" ) );
    CheckBox_ShowVGrid->setText( tr( "Show Vertical Grid Lines" ) );
    TextLabel6_2_2_2_2->setText( tr( "Grid Line Gap" ) );
    LineEdit_GridSize->setText( tr( "1" ) );
    tabWidget->changeTab( tab_2, tr( "Look and Feel" ) );
    TextLabel1_4_2_2_2_2->setText( tr( "Tag Window:" ) );
    CheckBox_TagWinFavouriteOnly->setText( tr( "Show only Favourite Tags" ) );
    QToolTip::add( CheckBox_TagWinFavouriteOnly, tr( "Set it if you want to view only a given subset of defined tags on items." ) );
    TextLabel1_4_2_2_2_2_2->setText( tr( "Item Window:" ) );
    CheckBox_SynchTabs->setText( tr( "Synchronize Tabs" ) );
    QToolTip::add( CheckBox_SynchTabs, tr( "If set, 2Dreams tries to keep syncronized the \"Selected\" item among the different tabs in the Item Window." ) );
    tabWidget->changeTab( tab_3, tr( "GUI Behavior" ) );
    TextLabel1_4_2_2_2_2_2_2->setText( tr( "DRAL Semantics:" ) );
    CheckBox_RelaxedEENode->setText( tr( "Coherence relaxed Enter/Exit Node commands (DRAL 1)" ) );
    QToolTip::add( CheckBox_RelaxedEENode, tr( "Commonly usefull in cutted drl files. In unsure leave set." ) );
    CheckBox_BackPropagate->setText( tr( "Backward Tag Propagation (DRAL 2)" ) );
    QToolTip::add( CheckBox_BackPropagate, tr( "Tag Semantic changed from DRAL 1 to DRAL 2.  For DRAL 2 drl files you can mimic DRAL 1 behavior setting this option. " ) );
    CheckBox_CompressMutable->setText( tr( "Compress Mutable Tags" ) );
    QToolTip::add( CheckBox_CompressMutable, tr( "Tag Semantic changed from DRAL 1 to DRAL 2.  For DRAL 2 drl files you can mimic DRAL 1 behavior setting this option. " ) );
    TextLabel1_4_2_2_2->setText( tr( "Automatic purges:" ) );
    CheckBox_AutoPurge->setText( tr( "Non-deleted items when reaching end of events file" ) );
    CheckBox_IncPurge->setText( tr( "Non-deleted items older than (cycles)" ) );
    QToolTip::add( CheckBox_IncPurge, tr( "Use With Caution! Can lead to lose of information." ) );
    CheckBox_MaxPurge->setText( tr( "Limit Number of Inflight (Live) Items" ) );
    QToolTip::add( CheckBox_MaxPurge, tr( "Use With Caution! Can lead to lose of information." ) );
    LineEdit_MaxInflight->setText( tr( "0" ) );
    tabWidget->changeTab( tab_4, tr( "DRAL" ) );
    buttonOk->setText( QString::null );
    buttonOk->setAccel( QKeySequence( QString::null ) );
    buttonCancel->setText( QString::null );
    buttonCancel->setAccel( QKeySequence( QString::null ) );
}

