#include "MosuanUiRightRail.h"
#include "MosuanUiState.h"
#include "MosuanUiPrimitives.h"
namespace MosuanUI {
static int railX(const RECT& r){return r.right-kRightRail-4;} static int slotY(int i){return 86+i*(kSlot+kGap);} static bool hit(POINT p,RECT r){return PtInRect(&r,p)!=FALSE;}
void drawRightRail(HDC dc,const RECT& rc){auto&s=state();int x=railX(rc),y=86;RECT bg{x-5,y-8,x+kRightRail+2,y+6+6*kSlot+5*kGap};fillRound(dc,bg,color(25,31,41),20);strokeRound(dc,bg,color(61,71,84),20);
for(int i=0;i<6;i++){RECT b{x,y+i*(kSlot+kGap),x+kSlot,y+i*(kSlot+kGap)+kSlot};fillRound(dc,b,s.favorites[i].valid?color(34,42,54):color(29,36,46),12);if(s.favorites[i].valid)icon(dc,s.favorites[i].command==ID_TOOL_LINE?10:6,b,s.selectedTool==s.favorites[i].command);else icon(dc,24,b);strokeRound(dc,b,color(60,72,89),12);}
RECT star{x,y+6*(kSlot+kGap)+6,x+kSlot,y+6*(kSlot+kGap)+6+kSlot};fillRound(dc,star,color(29,36,46),12);icon(dc,19,star);strokeRound(dc,star,color(60,72,89),12);
RECT layer{x,y+7*(kSlot+kGap)+10,x+kSlot,y+7*(kSlot+kGap)+10+kSlot};fillRound(dc,layer,s.layersOpen?color(55,116,210):color(29,36,46),12);icon(dc,20,layer,s.layersOpen);strokeRound(dc,layer,color(60,72,89),12);
RECT settings{x,y+8*(kSlot+kGap)+14,x+kSlot,y+8*(kSlot+kGap)+14+kSlot};fillRound(dc,settings,s.settingsOpen?color(55,116,210):color(29,36,46),12);icon(dc,21,settings,s.settingsOpen);strokeRound(dc,settings,color(60,72,89),12);}

bool handleRightRailClick(POINT p,const RECT& rc){auto&s=state();int x=railX(rc),y=86;for(int i=0;i<6;i++){RECT b{x,y+i*(kSlot+kGap),x+kSlot,y+i*(kSlot+kGap)+kSlot};if(hit(p,b)){if(s.favorites[i].valid){s.selectedTool=s.favorites[i].command;SendMessageW(s.main,WM_COMMAND,MAKEWPARAM(s.selectedTool,BN_CLICKED),0);}return true;}}
RECT star{x,y+6*(kSlot+kGap)+6,x+kSlot,y+6*(kSlot+kGap)+6+kSlot};if(hit(p,star)){for(auto&slot:s.favorites)if(!slot.valid){slot.valid=true;slot.command=s.selectedTool;return true;}s.favorites.back()={true,s.selectedTool};return true;}
RECT layer{x,y+7*(kSlot+kGap)+10,x+kSlot,y+7*(kSlot+kGap)+10+kSlot};if(hit(p,layer)){s.layersOpen=!s.layersOpen;s.settingsOpen=false;return true;}
RECT settings{x,y+8*(kSlot+kGap)+14,x+kSlot,y+8*(kSlot+kGap)+14+kSlot};if(hit(p,settings)){s.settingsOpen=!s.settingsOpen;s.layersOpen=false;return true;}return false;}
}
