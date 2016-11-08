#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <gdk/gdkkeysyms.h>
#include <sys/stat.h>
GError *error = NULL;
GtkWidget *window, *statusbar1,*statusbar2,*statusbar3, *drawarea, *label, *entry;
gint context_id1,context_id2,context_id3;
int xa, ya, xb, yb, xc, yc, r, w , h,rw, rh, a1, a2, lw = 2;
GdkGC *gc,*gcDash;
GdkColor fgcolor, bgcolor, erasecolor;
GdkFont *gdkfont;
PangoFontDescription *fontDesc;
const gchar *fontName;
GdkCursor *cursor;
gchar *filename;
GdkPixbuf *pixbuf100,*pixbufcopy,*pixbuf[10];
GdkPixmap *pixmap;
int zoomed=0,iscut=0,cancel,drawend=0,fundo=0,fredo=0,cundo=0,credo=0;
char sxy[100],swh[100],fsize[10];

enum				/*定义图形的类型 */
{
  PIX_DRAW,
  LINE_DRAW,
  RECT_DRAW,
  CIRCLE_DRAW,
  TEXT_DRAW,
  PAINT_DRAW,
  ERASE_DRAW,
  SELECT_DRAW,
  CLIP_DRAW  
} draw_type;

void new (GtkWidget * newitem, gpointer data)
{
  g_message ("newing...");
  gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, 0, 0, drawarea->allocation.width, drawarea->allocation.height);
  gtk_widget_queue_draw (drawarea);
  gtk_window_set_title (GTK_WINDOW (window), "未命名 - 海天鹰画图");
  filename = "未命名";
}

void openfile (GtkWidget * openitem, gpointer data)
{
  g_message ("opening...");
  GtkWidget *FCD, *gtkimage;
  GdkImage *gdkimage;
  char title[100];
  GtkFileFilter *filter;
  FCD = gtk_file_chooser_dialog_new ("打开图片", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT, NULL);
  filter = gtk_file_filter_new ();
  gtk_file_filter_add_pixbuf_formats (filter);
  gtk_file_filter_set_name (filter, "图片文件");
  //gtk_file_filter_add_pattern(filter,"*.jpg");
  //gtk_file_filter_add_pattern(filter,"*.png");
  //gtk_file_filter_add_pattern(filter,"*.bmp");
  //gtk_file_filter_add_pattern(filter,"*.gif");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
  if (gtk_dialog_run (GTK_DIALOG (FCD)) == GTK_RESPONSE_ACCEPT)
  {
    //isSelectAll=0;
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (FCD));
    g_message (filename);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (filename, &error);
    pixbuf100=pixbuf;
    if (error != NULL)
	  {
	    g_message ("gdk_pixbuf_new_from_file error:%s", error->message);
	  }
    int wpb = gdk_pixbuf_get_width (pixbuf);
    int hpb = gdk_pixbuf_get_height (pixbuf);   
    g_message ("%d X %d", wpb, hpb);
    gtk_widget_set_size_request (drawarea, wpb, hpb);
    pixmap = gdk_pixmap_new (GDK_DRAWABLE (drawarea->window), wpb, hpb, -1);
    gdk_draw_pixbuf (pixmap, gc, pixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
    gtk_widget_queue_draw (drawarea);
    char *ptr = strrchr (filename, '/');
    sprintf (title, "%s - 海天鹰画图", ptr + 1);    
    gtk_window_set_title (GTK_WINDOW (window), title);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, "图片载入完毕");
  }
  gtk_widget_destroy (FCD);  
}

char *B2G(int b)
{
 static char g[10];
 if(b>999999999){
  sprintf(g,"%.2fGB",(float)b/1073741824);
 }else{
  if(b>999999){
   sprintf(g,"%.2fMB",(float)b/1048576);
  }else{
   if(b>999){
    sprintf(g,"%.2fKB",(float)b/1024);
   }else{
    sprintf(g,"%dB",b);
   }
  }
 }
 return g;
}

int file_size2(char* filename)  
{  
    struct stat statbuf;  
    stat(filename,&statbuf);  
    int size=statbuf.st_size;    
    return size;  
}

void save (GtkWidget * saveitem, gpointer data)
{
  char title[100];
  int width, height;
  if (strcmp (gtk_window_get_title (GTK_WINDOW (window)), "未命名 - 海天鹰画图") == 0)
  {
   GtkWidget *FCD = gtk_file_chooser_dialog_new ("保存图片", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, GTK_STOCK_SAVE,	GTK_RESPONSE_ACCEPT, NULL);
   GtkFileFilter *filter;
   filter = gtk_file_filter_new ();
   gtk_file_filter_set_name (filter, "*.jpg");
   gtk_file_filter_add_pattern (filter, "*.jpg");
   gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
   filter = gtk_file_filter_new ();
   gtk_file_filter_set_name (filter, "*.png");
   gtk_file_filter_add_pattern (filter, "*.png");
   gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
   filter = gtk_file_filter_new ();
   gtk_file_filter_set_name (filter, "*.bmp");
   gtk_file_filter_add_pattern (filter, "*.bmp");
   gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
   filter = gtk_file_filter_new ();
   gtk_file_filter_set_name (filter, "*.gif");
   gtk_file_filter_add_pattern (filter, "*.gif");
   gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
   if (gtk_dialog_run (GTK_DIALOG (FCD)) == GTK_RESPONSE_ACCEPT)
	  {
	    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (FCD));
	    g_message ("new %s", filename);
	    char *ptr = strrchr (filename, '/');
	    sprintf (title, "%s - 海天鹰画图", ptr + 1);
	    gtk_window_set_title (GTK_WINDOW (window), title);
	  }
    gtk_widget_destroy (FCD);
  }
  g_message ("save %s", filename);
  //gdk_drawable_get_size (GDK_DRAWABLE (drawarea->window), &width, &height);
  //GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, drawarea->window, NULL, 0, 0, 0, 0, width, height);
  GdkPixbuf *pixbuf =	gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0,	0,  drawarea->allocation.width,  drawarea->allocation.height);
  gdk_pixbuf_save (pixbuf, filename, "jpeg", NULL, "quality", "100", NULL);
  strcpy(fsize,B2G(file_size2(filename)));
  sprintf (sxy, "Saved, %s", fsize);
  gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
}

void saveAs (GtkWidget * saveitem, gpointer data)
{
  g_message ("save as...");
  char title[100];
  int width, height;
  GtkWidget *FCD = gtk_file_chooser_dialog_new ("保存图片", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,	GTK_STOCK_SAVE,	GTK_RESPONSE_ACCEPT, NULL);
  GtkFileFilter *filter;
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, "*.jpg");
  gtk_file_filter_add_pattern (filter, "*.jpg");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, "*.png");
  gtk_file_filter_add_pattern (filter, "*.png");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, "*.bmp");
  gtk_file_filter_add_pattern (filter, "*.bmp");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
  filter = gtk_file_filter_new ();
  gtk_file_filter_set_name (filter, "*.gif");
  gtk_file_filter_add_pattern (filter, "*.gif");
  gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (FCD), filter);
  if (gtk_dialog_run (GTK_DIALOG (FCD)) == GTK_RESPONSE_ACCEPT)
  {
    filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (FCD));
    g_message (filename);
    //gdk_drawable_get_size (GDK_DRAWABLE (drawarea->window), &width, &height);
    //GdkPixbuf *pixbuf =	gdk_pixbuf_get_from_drawable (NULL, drawarea->window, NULL, 0, 0, 0, 0, width, height);
    GdkPixbuf *pixbuf =	gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0,	0,  drawarea->allocation.width,  drawarea->allocation.height);
    gdk_pixbuf_save (pixbuf, filename, "jpeg", NULL, "quality", "100", NULL);
    strcpy(fsize,B2G(file_size2(filename)));
    sprintf (sxy, "Saved, %s", fsize);
    gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
    char *ptr = strrchr (filename, '/');
    sprintf (title, "%s - 海天鹰画图", ptr + 1);
    gtk_window_set_title (GTK_WINDOW (window), title);
  }
  gtk_widget_destroy (FCD);
}

void about (GtkWidget * aboutitem, gpointer data)
{
  g_message ("about...");
  GtkWidget *dialog;
  const gchar *authors[] = { "sonichy@163.com", NULL };
  const gchar *documenters[] = { "tianzhch@gmail.com","http://code.google.com/p/gnome-paint/", NULL };
  dialog = gtk_about_dialog_new ();
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dialog),TRUE);
  //gtk_widget_set_size_request(GTK_WIDGET(window),200,100);
  gtk_about_dialog_set_logo (GTK_ABOUT_DIALOG (dialog), NULL);
  gtk_about_dialog_set_name (GTK_ABOUT_DIALOG (dialog), "海天鹰画图");
  gtk_about_dialog_set_version (GTK_ABOUT_DIALOG (dialog), "1.0");
  gtk_about_dialog_set_copyright (GTK_ABOUT_DIALOG (dialog), "Copyright © 2016 黄颖");
  gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (dialog), "海天鹰画图是Linux平台下一个基于GTK2的小巧易用的画图工具。");
  gtk_about_dialog_set_website (GTK_ABOUT_DIALOG (dialog), "http://sonichy.96.lt/");
  gtk_about_dialog_set_authors (GTK_ABOUT_DIALOG (dialog), authors);
  gtk_about_dialog_set_documenters (GTK_ABOUT_DIALOG (dialog), documenters);
  gtk_about_dialog_set_translator_credits (GTK_ABOUT_DIALOG (dialog), "黄颖\nsonichy");
  gtk_about_dialog_set_license (GTK_ABOUT_DIALOG (dialog), "GNU GPL");
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void changelog (GtkWidget * widget, GtkWindow * parent)
{
  g_message ("changelog...");
  GtkWidget *dialog, *label, *scrolled, *vbox;
  dialog = gtk_dialog_new_with_buttons ("更新日志", parent, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
  //gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
  gtk_widget_set_size_request (dialog, 400, 300);
  gtk_window_set_resizable (GTK_WINDOW (GTK_DIALOG (dialog)), FALSE);
  label = gtk_label_new ("1.0\n2016-02\n加入图片控件，加入菜单，加入关于对话框，通过打开菜单弹出文件选择框载入图片，载入图片后从图片路径中提取图片名在窗口标题显示，增加状态栏，图片控件最大化，增加图片滚动，图片控件左上对齐，增加更新日志，增加单选图标工具栏。\n2016-03\n点击工具栏按钮随机位置画线、框、弧、文字。增加清屏。取到GdkGC，设置颜色。使用颜色按钮，颜色选择对话框来改变颜色。修改绘图区背景为白色。实现鼠标拖动绘图。\ngdk_draw_text无法修改字体无法显示中文，使用gdk_draw_layout解决！增加文字内容设置框。绘图区载入图片成功，但是修改绘图区大小，图片就画不出了。\n修复画框形、圆形的方向问题。实现设置线宽。实现擦除功能。增加绘图工具鼠标指针。菜单支持图标和快捷键。\n实现保存图片。实现保存、另存为对话框。\n解决窗口从最小化恢复，绘图区清空的问题！\n绘图区按打开图片大小分配！\ngtk_widget_add_events 添加的事件，GDK_POINTER_MOTION_MASK 改为 GDK_BUTTON1_MOTION_MASK ，解决铅笔和擦除工具，鼠标按住左键才绘图，而不是移动就绘图。铅笔实现连续绘图，但是有锯齿。\n初步实现绘图预览，但是闪屏严重。\npixbuf保存窗口，如果图片过大会有很多色块，改为保存pixmap解决。\n修改文字内容不再需要确定。\n实现放大、缩小、原始大小功能，但是放大一级就出错。\n设置编辑、图像子菜单。\n实现修改画布大小。\n实现选区删除、复制、剪切、粘贴，但是不能移动。\n分出3个状态栏。\n解决绘图预览闪烁问题。实现显示橡皮擦轮廓。\n实现剪裁功能。\n2016-04\n增加使用快捷键移动选框和选框边框。\n帮助菜单增加快捷键说明。\n修复全选BUG，并设置快捷键。\n实现撤销一步，并设置快捷键。\n实现撤销、重做多步！\n增加改变选框边界的约束。\n增加设置图片为壁纸。\n2016-05\n自定义比例缩放，放大会出错。\n2016-06\n预设pixmap大小，修复放大出错的问题。\n2016-08\n取消边界约束。丰富状态栏信息。保存时显示文件大小。");
  gtk_label_set_line_wrap (GTK_LABEL (label), TRUE);
  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy ((GtkScrolledWindow *) (scrolled),	GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), label);
  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), scrolled);
  gtk_widget_show_all (dialog);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void accelKeyList (GtkWidget * widget, GtkWindow * parent)
{
  g_message ("accelKey...");
  GtkWidget *dialog, *clist, *scrolled, *vbox;
  int i;
  dialog = gtk_dialog_new_with_buttons ("海天鹰画图", parent, GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_OK, NULL);
  //gtk_dialog_set_has_separator(GTK_DIALOG(dialog),FALSE);
  gtk_widget_set_size_request (dialog, 400, 300);
  gtk_window_set_resizable (GTK_WINDOW (GTK_DIALOG (dialog)), FALSE);
  //clist=gtk_clist_new(2);
  gchar *titles[2] = { "功能", "快捷键" };
  clist = gtk_clist_new_with_titles( 2, titles);
  gtk_clist_set_column_width(GTK_CLIST(clist),0,100);
  //gtk_clist_set_column_justification  (GTK_CLIST(clist),0,GTK_JUSTIFY_CENTER);
  gchar *list[12][2] = {{"选区上移","Alt + Up"},{"选区下移","Alt + Down"},{"选区左移","Alt + Left"},{"选区右移","Alt + Right"},{"选区上边上移","Shift + Up"},{"选区上边下移","Shift + Down"},{"选区左边左移","Shift + Left"},{"选区左边右移","Shift + Right"},{"选区下边上移","Ctrl + Up"},{"选区下边下移","Ctrl + Down"},{"选区右边左移","Ctrl + Left"},{"选区右边右移","Ctrl + Right"}};
  for(i=0;i<12;i++)gtk_clist_append(GTK_CLIST(clist), list[i]);
  //scrolled = gtk_scrolled_window_new (NULL, NULL);
  //gtk_scrolled_window_set_policy ((GtkScrolledWindow *) (scrolled),	GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  //gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), clist);
  //gtk_container_add(GTK_CONTAINER(scrolled),clist);
  gtk_box_pack_start_defaults (GTK_BOX (GTK_DIALOG (dialog)->vbox), clist);
  gtk_widget_show_all (dialog);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dialog),TRUE);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy (dialog);
}

void drawPoint (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawPixel...");
  draw_type = PIX_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("pencil.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void drawLine (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawLine...");
  draw_type = LINE_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("line.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void drawRect (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawRect...");
  draw_type = RECT_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("rect.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void drawArc (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawArc...");
  draw_type = CIRCLE_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("ellipse.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void drawText (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawText...");
  draw_type = TEXT_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("text.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void drawPaint (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawPaint...");
  draw_type = PAINT_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("paint.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void zoomout (GtkWidget * widget, gpointer func_data)
{
  g_message ("zoomout...");
  int dw=drawarea->allocation.width;
  int dh=drawarea->allocation.height;
  GdkPixbuf *pixbuf =	gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0,	0, dw, dh);
  if(!zoomed)pixbuf100=pixbuf;
  zoomed=1;  
  GdkPixbuf *pixbufScaled=gdk_pixbuf_scale_simple (pixbuf,dw*0.9/1, dh*0.9/1,GDK_INTERP_HYPER);
  gdk_draw_pixbuf (pixmap, gc, pixbufScaled, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
  gtk_widget_set_size_request (drawarea, dw*0.9/1, dh*0.9/1);
  gtk_widget_queue_draw (drawarea);
}

void zoomin (GtkWidget * widget, gpointer func_data)
{
  g_message ("zoomin...");
  int dw=drawarea->allocation.width;
  int dh=drawarea->allocation.height;
  GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0, 0, dw, dh);
  if(!zoomed)pixbuf100=pixbuf;
  zoomed=1;
  //init pixmap
  pixmap = gdk_pixmap_new (GDK_DRAWABLE (drawarea->window), dh*1.1/1, dh*1.1/1, -1);
  GdkPixbuf *pixbufScaled = gdk_pixbuf_scale_simple (pixbuf,dw*1.1/1, dh*1.1/1, GDK_INTERP_HYPER);
  gdk_draw_pixbuf (pixmap, gc, pixbufScaled, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
  gtk_widget_set_size_request (drawarea, dw*1.1/1, dh*1.1/1);
  gtk_widget_queue_draw (drawarea);
}

void zoom100 (GtkWidget * widget, gpointer func_data)
{
  g_message ("zoom100:zoomed=%d",zoomed);
  if(zoomed){
    int wpb = gdk_pixbuf_get_width (pixbuf100);
    int hpb = gdk_pixbuf_get_height (pixbuf100);
    gtk_widget_set_size_request (drawarea, wpb, hpb);
    pixmap = gdk_pixmap_new (GDK_DRAWABLE (drawarea->window), wpb, hpb, -1);
    gdk_draw_pixbuf (pixmap, gc, pixbuf100, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
    gtk_widget_queue_draw (drawarea);   
  }
}

void drawErase (GtkWidget * widget, gpointer func_data)
{
  g_message ("drawErase...");
  draw_type = ERASE_DRAW;
  gdk_gc_set_rgb_fg_color (gc, &erasecolor);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("eraser.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void drawSelect(){
  g_message ("drawSelect...");
  draw_type = SELECT_DRAW;
  //gdk_gc_set_rgb_fg_color (gc, &fgcolor);
  //gdk_gc_set_line_attributes (gc, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_NOT_LAST,GDK_JOIN_MITER);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file ("rectselect.png", &error);
  cursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default (), pixbuf, 0, 15);
  gdk_window_set_cursor (drawarea->window, cursor);
}

void setFGColor (GtkWidget * widget, gpointer func_data)
{
  g_message ("setFGColor...");
  gtk_color_button_get_color (GTK_COLOR_BUTTON (widget), &fgcolor);
  gdk_gc_set_rgb_fg_color (gc, &fgcolor);
}

void setFont (GtkWidget * widget, gpointer func_data)
{
  fontName = gtk_font_button_get_font_name (GTK_FONT_BUTTON (widget));
  g_message (fontName);
  fontDesc = pango_font_description_from_string (fontName);
  gdkfont = gdk_font_from_description (fontDesc);
}

void setLineWidth (GtkWidget * widget, gpointer func_data)
{
  lw = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
  //g_message ("LineWidth=%d", lw);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_NOT_LAST,	GDK_JOIN_MITER);
}

void savePixbufArray()
{
  g_print("savePixbufArray\n");
  int i;
  for(i=9;i>0;i--)
  {
    pixbuf[i]=pixbuf[i-1];
    //g_print("%d = %d\n",i,i-1);
  }
  pixbuf[0]=gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0,	0, drawarea->allocation.width, drawarea->allocation.height);
}

void draw_start (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
  zoomed=0;
  cancel=0;
  drawend=0;
  switch (event->button)
  {
    case 1:      
      if(draw_type!=SELECT_DRAW)savePixbufArray();
      xa = (gint) event->x;
      ya = (gint) event->y;
      switch (draw_type)
      {
        case PIX_DRAW:
          xc = xa;
          yc = ya;
          break;
        case TEXT_DRAW:
          g_message ("TEXT_DRAW");
          const gchar *text = gtk_entry_get_text (GTK_ENTRY (entry));
          //gdk_draw_text(drawarea->window,gdkfont,gc,xa,ya,text,strlen(text));
          PangoLayout *layout =	gtk_widget_create_pango_layout (drawarea, text);
          pango_layout_set_font_description (layout, fontDesc);
          gdk_draw_layout (pixmap, gc, xa, ya, layout);
          gtk_widget_queue_draw (drawarea);
          sprintf (sxy, "X1,Y1: %d,%d", xa, ya);
          gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
          break;
       case LINE_DRAW:
         sprintf (sxy, "X1,Y1: %d,%d", xa, ya);
         gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
         break;
       case CIRCLE_DRAW:
         sprintf (sxy, "X1,Y1: %d,%d", xa, ya);
         gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
         break;
       case RECT_DRAW:
         sprintf (sxy, "X1,Y1: %d,%d", xa, ya);
         gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
         break;  
       case PAINT_DRAW:
         break;
      }
      break;
  }
}

void draw_move (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
  zoomed=0;  
  xb = (gint) event->x;
  yb = (gint) event->y;
  //if(xb<0)xb=0;
  //if(yb<0)yb=0;
  //if(xb>drawarea->allocation.width)xb=drawarea->allocation.width;
  //if(yb>drawarea->allocation.height)yb=drawarea->allocation.height;
  w = abs (xb - xa);
  h = abs (yb - ya);  
  switch (draw_type)
  {
    case PIX_DRAW:
      gdk_draw_line (pixmap, gc, xc, yc, xb, yb);
      sprintf (sxy, "X2,Y2: %d,%d", xb, yb);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar2), context_id2, sxy);
      break;
    case ERASE_DRAW:
      gdk_draw_arc (pixmap, drawarea->style->white_gc, TRUE, xb-lw/2, yb-lw/2, lw, lw, 0, 360 * 64);
      break;
    case LINE_DRAW:
      sprintf (sxy, "X2,Y2: %d,%d", xb, yb);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar2), context_id2, sxy);
      break;
    case CIRCLE_DRAW:
      sprintf (sxy, "X2,Y2: %d,%d", xb, yb);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar2), context_id2, sxy);
      break;
    case RECT_DRAW:
      sprintf (swh, "W,H: %d,%d", w, h);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar2), context_id2, swh);
      break;
  }
  gtk_widget_queue_draw (drawarea);
  xc = xb;
  yc = yb;
}

void draw_end (GtkWidget * widget, GdkEventButton * event, gpointer data)
{
  zoomed=0;
  drawend=1;
  switch (event->button)
  {    
    case 1:
      //g_print("button1 clicked !\n");
      g_print("cancle=%d\n",cancel);
      if(!cancel){
      xb = (gint) event->x;
      yb = (gint) event->y;
      //if(xb<0)xb=0;
      //if(yb<0)yb=0;
      //if(xb>drawarea->allocation.width)xb=drawarea->allocation.width;
      //if(yb>drawarea->allocation.height)yb=drawarea->allocation.height;
      w = abs (xb - xa);
      h = abs (yb - ya);
      switch (draw_type)
      {
        case PIX_DRAW:
          xc = xb;
          yc = yb;
          break;
        case LINE_DRAW:
          g_message ("LINE_DRAW");
          gdk_draw_line (pixmap, gc, xa, ya, xb, yb);
          gtk_widget_queue_draw (drawarea);
          break;
        case RECT_DRAW:
          g_message ("RECT_DRAW");          
          if (xa < xb && ya < yb) gdk_draw_rectangle (pixmap, gc, FALSE, xa, ya, w, h);
          if (xa > xb && ya > yb)	gdk_draw_rectangle (pixmap, gc, FALSE, xb, yb, w, h);
          if (xa < xb && ya > yb)	gdk_draw_rectangle (pixmap, gc, FALSE, xa, yb, w, h);
          if (xa > xb && ya < yb)	gdk_draw_rectangle (pixmap, gc, FALSE, xb, ya, w, h);
          gtk_widget_queue_draw (drawarea);
          break;
        case SELECT_DRAW:
          //isSelectAll=0;
          //if (xa < xb && ya < yb) gdk_draw_rectangle (widget->window, gc, FALSE, xa, ya, w-1, h-1);
          //if (xa > xb && ya > yb)	gdk_draw_rectangle (widget->window, gc, FALSE, xb, yb, w-1, h-1);
          //if (xa < xb && ya > yb)	gdk_draw_rectangle (widget->window, gc, FALSE, xa, yb, w-1, h-1);
          //if (xa > xb && ya < yb)	gdk_draw_rectangle (widget->window, gc, FALSE, xb, ya, w-1, h-1);
          break;
        case CIRCLE_DRAW:
          g_message ("CIRCLE_DRAW");         
          if (xa < xb && ya < yb)	gdk_draw_arc (pixmap, gc, FALSE, xa, ya, w, h, 0, 360 * 64);
          if (xa > xb && ya > yb)	gdk_draw_arc (pixmap, gc, FALSE, xb, yb, w, h, 0, 360 * 64);
          if (xa < xb && ya > yb)	gdk_draw_arc (pixmap, gc, FALSE, xa, yb, w, h, 0, 360 * 64);
          if (xa > xb && ya < yb)	gdk_draw_arc (pixmap, gc, FALSE, xb, ya, w, h, 0, 360 * 64);
          gtk_widget_queue_draw (drawarea);
          break;
        case ERASE_DRAW:
          gtk_widget_queue_draw (drawarea);
          break;
      }
      
      }
      break;
  } 
}

gboolean expose_event_callback (GtkWidget * widget, GdkEventExpose * event,	gpointer data)
{
  //g_message ("expose_event");
  int wa=widget->allocation.width;
  int ha=widget->allocation.height;
  gdk_draw_drawable (drawarea->window, gc, pixmap, 0, 0, 0, 0, wa, ha);
  if(fundo){fundo=0;}else{
  switch (draw_type)
  {
    case LINE_DRAW:
      gdk_draw_line (widget->window, gc, xa, ya, xb, yb);
      break;
    case RECT_DRAW:
      if (xa < xb && ya < yb) gdk_draw_rectangle (widget->window, gc, FALSE, xa, ya, w, h);
      if (xa > xb && ya > yb)	gdk_draw_rectangle (widget->window, gc, FALSE, xb, yb, w, h);
      if (xa < xb && ya > yb)	gdk_draw_rectangle (widget->window, gc, FALSE, xa, yb, w, h);
      if (xa > xb && ya < yb)	gdk_draw_rectangle (widget->window, gc, FALSE, xb, ya, w, h);
      break;
    case CIRCLE_DRAW:      
      if (xa < xb && ya < yb)	gdk_draw_arc (widget->window, gc, FALSE, xa, ya, w, h, 0, 360 * 64);
      if (xa > xb && ya > yb)	gdk_draw_arc (widget->window, gc, FALSE, xb, yb, w, h, 0, 360 * 64);
      if (xa < xb && ya > yb)	gdk_draw_arc (widget->window, gc, FALSE, xa, yb, w, h, 0, 360 * 64);
      if (xa > xb && ya < yb)	gdk_draw_arc (widget->window, gc, FALSE, xb, ya, w, h, 0, 360 * 64);
      break;    
    case SELECT_DRAW:      
      if (xa < xb && ya < yb){ gdk_draw_rectangle (widget->window, gcDash, FALSE, xa, ya, w-1, h-1);sprintf (sxy, "X,Y: %d,%d",xa,ya);}
      if (xa > xb && ya > yb){ gdk_draw_rectangle (widget->window, gcDash, FALSE, xb, yb, w-1, h-1);sprintf (sxy, "X,Y: %d,%d",xb,yb);}
      if (xa < xb && ya > yb){ gdk_draw_rectangle (widget->window, gcDash, FALSE, xa, yb, w-1, h-1);sprintf (sxy, "X,Y: %d,%d",xa,yb);}
      if (xa > xb && ya < yb){ gdk_draw_rectangle (widget->window, gcDash, FALSE, xb, ya, w-1, h-1);sprintf (sxy, "X,Y: %d,%d",xb,ya);}
      gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, sxy);
      //char swh[100];
      sprintf (swh, "W,H: %d X %d",w,h);
      gtk_statusbar_push (GTK_STATUSBAR (statusbar2), context_id2, swh);
      break;
    case ERASE_DRAW:
      if(!drawend) gdk_draw_arc (drawarea->window, drawarea->style->black_gc, FALSE, xb-lw/2, yb-lw/2, lw, lw, 0, 360 * 64); 
      break;  
  }
  }
  //char swh[100];
  sprintf (swh, "%d X %d",wa,ha);
  gtk_statusbar_push (GTK_STATUSBAR (statusbar3), context_id3, swh);
  return TRUE;
}

int my_configure_event (GtkWidget * widget, GdkEventConfigure * event)
{
  g_message ("configure_event");
  return 0;
}

int window_state (GtkWidget * widget, gpointer data)
{
  g_message ("window_state_event");
  switch (gdk_window_get_state (GDK_WINDOW (widget->window)))
  {
    case GDK_WINDOW_STATE_ICONIFIED:
      g_message ("the window is minimized.");
      break;
  }
  return 0;
}

void stretch(){
  g_print("stretch\n");  
  GtkWidget *dialog,*hbox,*vbox,*label,*spinw,*spinh;
  GtkObject *adjustment;
  int result;
  dialog = gtk_dialog_new_with_buttons("拉伸",NULL,GTK_DIALOG_MODAL, GTK_STOCK_OK,GTK_RESPONSE_OK, GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, NULL);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dialog),TRUE);
  vbox = gtk_vbox_new (FALSE, 0);  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("宽度：");
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE, TRUE,10);  
  adjustment = gtk_adjustment_new (100, 10, 1000, 1, 1, 0);
  spinw = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox),spinw,FALSE, TRUE,0);
  label = gtk_label_new ("%");
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE, TRUE,10);
  gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE, TRUE,10);
  hbox = gtk_hbox_new (FALSE, 0);  
  label = gtk_label_new ("高度：");
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE, TRUE,10);
  adjustment = gtk_adjustment_new (100, 10, 1000, 1, 1, 0);
  spinh = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 1, 0);
  gtk_box_pack_start(GTK_BOX(hbox),spinh,FALSE, TRUE,0);
  label = gtk_label_new ("%");
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE, TRUE,10);
  gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE, TRUE,10);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),vbox);
  gtk_widget_show_all(dialog);
  result = gtk_dialog_run(GTK_DIALOG(dialog));
  switch(result){
    case GTK_RESPONSE_NONE:
      // -1，对话框被窗口管理器销毁或通过gtk_widget_destroy()有其他程序销毁。如果构件没有设置一个相应标识，会返回它。
      g_print("GTK_RESPONSE_NONE\n");
      break;
    case GTK_RESPONSE_REJECT:
      // -2 ，这个标识没有和对话框内置的任何按钮相关联，可以任意使用它。
      g_print("GTK_RESPONSE_REJECT\n");
      break;
    case GTK_RESPONSE_ACCEPT:
      //* -3 ，这个标识没有和对话框内置的任何按钮相关联，可以任意使用它
      g_print("GTK_RESPONSE_ACCEPT\n");
      break;
    case GTK_RESPONSE_DELETE_EVENT:
      //* -4 ，每个对话框都会自动连接到delete-event信号，当gtk_dialog_run() 运行时，这个标识会被返回，而且delete-event会被终止、不会像往常那样销毁窗口 
      g_print("GTK_RESPONSE_DELETE_EVENT\n");
      break;
    case GTK_RESPONSE_OK:
      // -5 ， GTK_STOCK_OK 被点击
      g_print("GTK_RESPONSE_OK\n");     
      int sw=gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spinw));
      int sh=gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spinh));
      g_print("Scale Width:\t%d%\n",sw);
      g_print("Scale Height:\t%d%\n",sh);
      int nw=drawarea->allocation.width*sw/100;
      int nh=drawarea->allocation.height*sh/100;
      g_print("New Width:\t%d\n",nw);
      g_print("New Height:\t%d\n",nh);     
      GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0, 0, drawarea->allocation.width, drawarea->allocation.height);
      //GdkPixbuf *pixbuf =	gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0,	0, dw, dh);
      pixmap = gdk_pixmap_new (GDK_DRAWABLE (drawarea->window), nw, nh, -1);
      GdkPixbuf *pixbufScaled = gdk_pixbuf_scale_simple (pixbuf,nw, nh, GDK_INTERP_HYPER);
      gdk_draw_pixbuf (pixmap, gc, pixbufScaled, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);           
      gtk_widget_set_size_request (drawarea, nw, nh);
      gtk_widget_queue_draw (drawarea);
      break;  
    case GTK_RESPONSE_CANCEL:  
      // -6 ， GTK_STOCK_CANCEL被点击 
      g_print("GTK_RESPONSE_CANCEL\n");  
      break;  
    case GTK_RESPONSE_CLOSE:  
      // -7 ，GTK_STOCK_CLOSE被点击
      g_print("GTK_RESPONSE_CLOSE\n");  
      break;  
    case GTK_RESPONSE_YES:  
      // -8 ，GTK_STOCK_YES被点击
      g_print("GTK_RESPONSE_YES\n");  
      break;  
    case GTK_RESPONSE_NO:  
      // -9 ，GTK_STOCK_NO被点击
      g_print("GTK_RESPONSE_NO\n"); 
      break;  
    case GTK_RESPONSE_APPLY:  
      // -10 ，GTK_STOCK_APPLY被点击
      g_print("GTK_RESPONSE_APPLY\n"); 
      break;  
    case GTK_RESPONSE_HELP:  
      // -11 ，GTK_STOCK_HELP被点击
      g_print("GTK_RESPONSE_HELP\n"); 
      break;  
    default:  
      g_print("something wrong!\n");  
      break;  
   }  
   gtk_widget_destroy(dialog); 
}

void rotate(){

}

void property(){
  GtkWidget *dialog,*hbox,*vbox,*label,*spinw,*spinh;
  GtkObject *adjustment;
  int result;
  dialog = gtk_dialog_new_with_buttons("属性",NULL,GTK_DIALOG_MODAL, GTK_STOCK_OK,GTK_RESPONSE_OK, GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL, NULL);
  gtk_window_set_skip_taskbar_hint(GTK_WINDOW(dialog),TRUE);
  vbox = gtk_vbox_new (FALSE, 0);  
  hbox = gtk_hbox_new (FALSE, 0);
  label = gtk_label_new ("宽度：");
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE, TRUE,10);  
  adjustment = gtk_adjustment_new (drawarea->allocation.width, 1, 5000, 1, 1, 0);
  spinw = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 0.5, 0);
  gtk_box_pack_start(GTK_BOX(hbox),spinw,FALSE, TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE, TRUE,10);
  hbox = gtk_hbox_new (FALSE, 0);  
  label = gtk_label_new ("高度：");
  gtk_box_pack_start(GTK_BOX(hbox),label,FALSE, TRUE,10);
  adjustment = gtk_adjustment_new (drawarea->allocation.height, 1, 5000, 1, 1, 0);
  spinh = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 0.5, 0);
  gtk_box_pack_start(GTK_BOX(hbox),spinh,FALSE, TRUE,0);
  gtk_box_pack_start(GTK_BOX(vbox),hbox,FALSE, TRUE,10);
  gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(dialog)->vbox),vbox); 
  gtk_widget_show_all(dialog);
  result = gtk_dialog_run(GTK_DIALOG(dialog));  
  switch(result){  
    case GTK_RESPONSE_NONE:
      // -1，对话框被窗口管理器销毁或通过gtk_widget_destroy()有其他程序销毁。如果构件没有设置一个相应标识，会返回它。
      g_print("GTK_RESPONSE_NONE\n");  
      break;  
    case GTK_RESPONSE_REJECT:  
      // -2 ，这个标识没有和对话框内置的任何按钮相关联，可以任意使用它。
      g_print("GTK_RESPONSE_REJECT\n");  
      break;  
    case GTK_RESPONSE_ACCEPT:  
      //* -3 ，这个标识没有和对话框内置的任何按钮相关联，可以任意使用它
      g_print("GTK_RESPONSE_ACCEPT\n");  
      break;  
    case GTK_RESPONSE_DELETE_EVENT:  
      //* -4 ，每个对话框都会自动连接到delete-event信号，当gtk_dialog_run() 运行时，这个标识会被返回，而且delete-event会被终止、不会像往常那样销毁窗口 
      g_print("GTK_RESPONSE_DELETE_EVENT\n");  
      break;
    case GTK_RESPONSE_OK:  
      // -5 ， GTK_STOCK_OK 被点击
      g_print("GTK_RESPONSE_OK\n");  
      GdkPixbuf *pixbuf =	gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0,	0, drawarea->allocation.width, drawarea->allocation.height);
      int dw=gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spinw));
      int dh=gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(spinh));
      g_print("Width:\t%d\n",dw);  
      g_print("Height:\t%d\n",dh);           
      pixmap = gdk_pixmap_new (GDK_DRAWABLE (drawarea->window),	dw, dh, -1);
      gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, 0, 0,	dw,	dh);
      gdk_draw_pixbuf (pixmap, gc, pixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
      gtk_widget_set_size_request (drawarea, dw, dh);
      gtk_widget_queue_draw (drawarea);
      break;  
    case GTK_RESPONSE_CANCEL:  
      // -6 ， GTK_STOCK_CANCEL被点击 
      g_print("GTK_RESPONSE_CANCEL\n");  
      break;  
    case GTK_RESPONSE_CLOSE:  
      // -7 ，GTK_STOCK_CLOSE被点击
      g_print("GTK_RESPONSE_CLOSE\n");  
      break;  
    case GTK_RESPONSE_YES:  
      // -8 ，GTK_STOCK_YES被点击
      g_print("GTK_RESPONSE_YES\n");  
      break;  
    case GTK_RESPONSE_NO:  
      // -9 ，GTK_STOCK_NO被点击
      g_print("GTK_RESPONSE_NO\n"); 
      break;  
    case GTK_RESPONSE_APPLY:  
      // -10 ，GTK_STOCK_APPLY被点击
      g_print("GTK_RESPONSE_APPLY\n"); 
      break;  
    case GTK_RESPONSE_HELP:  
      // -11 ，GTK_STOCK_HELP被点击
      g_print("GTK_RESPONSE_HELP\n"); 
      break;  
    default:  
      g_print("something wrong!\n");  
      break;  
   }  
   gtk_widget_destroy(dialog);
}

void undo()
{  
  fundo=1;
  if(cundo<10 && pixbuf[cundo]!=0){    
    g_print("undo pixbuf[%d]\n",cundo);
    gdk_draw_pixbuf (pixmap, gc, pixbuf[cundo], 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
    if(cundo<9){cundo++;}
    gtk_widget_queue_draw (drawarea);
  }
}

void redo()
{  
  fredo=1;  
  if(cundo>-1 && pixbuf[cundo]!=0){    
    if(cundo>0){cundo--;}
    g_print("redo pixbuf[%d]\n",cundo);
    gdk_draw_pixbuf (pixmap, gc, pixbuf[cundo], 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);    
    gtk_widget_queue_draw (drawarea);
  }
}

void copy(){
  iscut=0;
  if (xa < xb && ya < yb) pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xa, ya, 0, 0, w, h);
  if (xa > xb && ya > yb)	pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xb, yb, 0, 0, w, h);
  if (xa < xb && ya > yb)	pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xa, yb, 0, 0, w, h);
  if (xa > xb && ya < yb)	pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xb, ya, 0, 0, w, h);
}

void cut(){
  iscut=1;
  //if(isSelectAll){pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, 0, 0, 0, 0, w, h);}
  //else{
  if (xa < xb && ya < yb) pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xa, ya, 0, 0, w, h);
  if (xa > xb && ya > yb)	pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xb, yb, 0, 0, w, h);
  if (xa < xb && ya > yb)	pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xa, yb, 0, 0, w, h);
  if (xa > xb && ya < yb)	pixbufcopy = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xb, ya, 0, 0, w, h);
  //}
}

void paste(){
  g_print("paste\n");
  gdk_draw_pixbuf (pixmap, gc, pixbufcopy, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
  if(iscut)gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, xa,ya,	w,h);
  gtk_widget_queue_draw (drawarea);
}

void selectAll(){
  draw_type = SELECT_DRAW;
  xa=0;
  ya=0;  
  xb=drawarea->allocation.width;
  yb=drawarea->allocation.height;
  w=xb-xa;
  h=yb-ya;
  gtk_widget_queue_draw (drawarea);
}

void delete(){  
  if (xa < xb && ya < yb) gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, xa,ya,w,h);
  if (xa > xb && ya > yb)	gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, xb,yb,	w,h);
  if (xa < xb && ya > yb)	gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, xa,yb,	w,h);
  if (xa > xb && ya < yb)	gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, xb,ya,	w,h);  
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  gtk_widget_queue_draw (drawarea);
}

void clip()
{
  g_print("clip\n");
  if(draw_type==SELECT_DRAW){
  GdkPixbuf *pixbuf;
  if (xa < xb && ya < yb) pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xa, ya, 0,	0,w,h);
  if (xa > xb && ya > yb)	pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xb, yb, 0, 0,w,h);
  if (xa < xb && ya > yb)	pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xa, yb, 0, 0,w,h);
  if (xa > xb && ya < yb)	pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, NULL, xb, ya, 0, 0,w,h);
  gdk_draw_pixbuf (pixmap, gc, pixbuf, 0, 0, 0, 0, -1, -1, GDK_RGB_DITHER_NORMAL, 0, 0);
  gtk_widget_set_size_request (drawarea, w, h);
  xa=0;
  ya=0;
  gtk_widget_queue_draw (drawarea);
  }
}

gboolean keyPress(GtkWidget *widget, GdkEventKey *event, gpointer data)
{
  g_print("%s pressed\n", gdk_keyval_name(event->keyval));
  return TRUE;
}

//move selected
void moveBottomUp()
{
 if(h>3){
  h-=1;
  gtk_widget_queue_draw (drawarea);
 }
}

void moveBottomDown()
{
  if(ya<yb && ya<(drawarea->allocation.height-h)){h+=1;}
  if(ya>yb && yb<(drawarea->allocation.height-h)){h+=1;}  
  gtk_widget_queue_draw (drawarea);
}

void moveRightLeft()
{
 if(w>3){
  w-=1;
  gtk_widget_queue_draw (drawarea);
 }
}

void moveRightRight()
{
  if(xa<xb && xa<(drawarea->allocation.width-w)){w+=1;}
  if(xa>xb && xb<(drawarea->allocation.width-w)){w+=1;}  
  gtk_widget_queue_draw (drawarea);
}

void moveLeftLeft()
{
 if(xa>0 && xb>0){
  if (xa < xb && ya < yb) {xa-=1;w+=1;}
  if (xa > xb && ya > yb)	{xb-=1;w+=1;}
  if (xa < xb && ya > yb)	{xa-=1;w+=1;}
  if (xa > xb && ya < yb)	{xb-=1;w+=1;}
  gtk_widget_queue_draw (drawarea);
 } 
}

void moveLeftRight()
{
 if(w>3){   
  if (xa < xb && ya < yb) {xa+=1;w-=1;}
  if (xa > xb && ya > yb)	{xb+=1;w-=1;}
  if (xa < xb && ya > yb)	{xa+=1;w-=1;}
  if (xa > xb && ya < yb)	{xb+=1;w-=1;}
  gtk_widget_queue_draw (drawarea);
 }
}

void moveTopUp()
{
 if(ya>0 && yb>0){
  if (xa < xb && ya < yb) {ya-=1;h+=1;}
  if (xa > xb && ya > yb)	{yb-=1;h+=1;}
  if (xa < xb && ya > yb)	{yb-=1;h+=1;}
  if (xa > xb && ya < yb)	{ya-=1;h+=1;}
  gtk_widget_queue_draw (drawarea);
 }
}

void moveTopDown()
{
 if(h>3){
  if (xa < xb && ya < yb) {ya+=1;h-=1;}
  if (xa > xb && ya > yb)	{yb+=1;h-=1;}
  if (xa < xb && ya > yb)	{yb+=1;h-=1;}
  if (xa > xb && ya < yb)	{ya+=1;h-=1;}
  gtk_widget_queue_draw (drawarea);
 }
}

void moveUp()
{
 if(ya>0 && yb>0){   
  if (xa < xb && ya < yb) {ya-=1;}
  if (xa > xb && ya > yb)	{yb-=1;}
  if (xa < xb && ya > yb)	{yb-=1;}
  if (xa > xb && ya < yb)	{ya-=1;}
  gtk_widget_queue_draw (drawarea);
 }
}

void moveDown()
{  
  if(ya<yb && ya<(drawarea->allocation.height-h)){ya+=1;}
  if(ya>yb && yb<(drawarea->allocation.height-h)){yb+=1;}  
  gtk_widget_queue_draw (drawarea);
}

void moveLeft()
{
 if(xa<xb && xa>0){xa-=1;}
 if(xa>xb && xb>0){xb-=1;}
 gtk_widget_queue_draw(drawarea);
}

void moveRight()
{
  if(xa<xb && xa<(drawarea->allocation.width-w)){xa+=1;}
  if(xa>xb && xb<(drawarea->allocation.width-w)){xb+=1;}
  gtk_widget_queue_draw (drawarea); 
}

void setWallpaper()
{
 //g_message("setWallpaper %d",window->prefs->desktop);
 if(filename!=NULL){
 //GString *cmd=g_string_new("gconftool-2 -s /desktop/gnome/background/picture_filename -t string \"");
 GString *cmd=g_string_new("gsettings set org.gnome.desktop.background picture-uri file://");
 g_string_append(cmd,filename);
 //g_string_append(cmd,"\"");
 g_message(cmd->str);
 system(cmd->str);
 }
}

int main (int argc, char *argv[])
{
  GtkWidget *scrolled, *vbox, *hbox, *mainbox, *menubox, *button, *mainmenu, *submenu, *filemenu, *helpmenu, *imagemenu,*editmenu, *image, *menuitem, *align, *toolbar, *spin,*sep;
  GtkToolItem *toolItem,*pencil, *paint, *rect, *ellipse, *line, *eraser, *move, *ttext, *bcolor,*rectselect;
  GtkObject *adjustment;
  GtkTooltips *tooltips;

  gtk_init (&argc, &argv);
  g_message ("GTK+ %d.%d.%d , GLib %d.%d.%d",GTK_MAJOR_VERSION,GTK_MINOR_VERSION,GTK_MICRO_VERSION,glib_major_version,glib_minor_version,glib_micro_version);
  
  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window), "未命名 - 海天鹰画图");
  gtk_window_set_default_icon (gdk_pixbuf_new_from_file ("icon.png", &error));
  gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
  gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
  g_signal_connect (window, "window_state_event", G_CALLBACK (window_state), NULL);
  g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  //g_signal_connect(window, "key-press-event", G_CALLBACK(keyPress), NULL);
  //AccelKey
  GClosure* closure = g_cclosure_new(moveBottomUp, 0, 0);
  GtkAccelGroup* accel_group = gtk_accel_group_new();
  gtk_accel_group_connect(accel_group,GDK_KEY_Up,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveBottomDown, 0, 0); 
  gtk_accel_group_connect(accel_group,GDK_KEY_Down,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveRightLeft, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Left,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveRightRight, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Right,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveLeftLeft, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Left,GDK_SHIFT_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveLeftRight, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Right,GDK_SHIFT_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveTopUp, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Up,GDK_SHIFT_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveTopDown, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Down,GDK_SHIFT_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveUp, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Up,GDK_MOD1_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveDown, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Down,GDK_MOD1_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveLeft, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Left,GDK_MOD1_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(moveRight, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_Right,GDK_MOD1_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(selectAll, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_a,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(undo, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_z,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  closure = g_cclosure_new(redo, 0, 0);
  gtk_accel_group_connect(accel_group,GDK_KEY_y,GDK_CONTROL_MASK,0,closure);
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  //menubar
  mainbox = gtk_vbox_new (FALSE, 0);
  menubox = gtk_vbox_new (FALSE, 0);
  gtk_container_add (GTK_CONTAINER (window), mainbox);
  mainmenu = gtk_menu_bar_new ();
  gtk_box_pack_start (GTK_BOX (menubox), mainmenu, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (mainbox), menubox, FALSE, TRUE, 0);
  //filemenu
  filemenu = gtk_menu_item_new_with_label ("文件");
  gtk_menu_bar_append (GTK_MENU_BAR (mainmenu), filemenu);
  submenu = gtk_menu_new ();
  //newitem=gtk_menu_item_new_with_label("新建");
  //GtkAccelGroup *accel_group = gtk_accel_group_new ();
  gtk_window_add_accel_group (GTK_WINDOW (window), accel_group);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_NEW, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (new), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  //openitem=gtk_menu_item_new_with_label("打开");
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_OPEN, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (openfile), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  //menuitem=gtk_menu_item_new_with_label("保存");
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (save), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_SAVE_AS, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (saveAs), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  //quititem=gtk_menu_item_new_with_label("退出");
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_QUIT, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (filemenu), submenu);
  //editmenu
  editmenu=gtk_menu_item_new_with_label ("编辑");
  gtk_menu_bar_append (GTK_MENU_BAR (mainmenu),	editmenu);
  submenu = gtk_menu_new ();
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_UNDO, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (undo), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_REDO, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (redo), (gpointer) window);  
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  sep=gtk_separator_menu_item_new();
  gtk_menu_append (GTK_MENU (submenu), sep);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_COPY, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (copy), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_CUT, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (cut), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_PASTE, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (paste), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_SELECT_ALL, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (selectAll), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_DELETE, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (delete), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (editmenu), submenu);
  //imagemenu 
  imagemenu=gtk_menu_item_new_with_label ("图像");
  gtk_menu_bar_append (GTK_MENU_BAR (mainmenu), imagemenu);
  submenu = gtk_menu_new();  
  menuitem = gtk_menu_item_new_with_label("设为壁纸");
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC(setWallpaper),NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_menu_item_new_with_label("翻转/旋转 Ctrl+R");
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (rotate), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_menu_item_new_with_label ("拉伸/扭曲 Ctrl+W");
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (stretch), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_PROPERTIES, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (property), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (imagemenu), submenu);  
  //helpmenu
  helpmenu = gtk_menu_item_new_with_label ("帮助");
  gtk_menu_bar_append (GTK_MENU_BAR (mainmenu), helpmenu);
  submenu = gtk_menu_new ();
  menuitem = gtk_menu_item_new_with_label ("快捷键");
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (accelKeyList), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  menuitem = gtk_menu_item_new_with_label ("更新日志");
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (changelog), (gpointer) window);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  //aboutitem=gtk_menu_item_new_with_label("关于");
  //accel_group=gtk_accel_group_new();
  menuitem = gtk_image_menu_item_new_from_stock (GTK_STOCK_ABOUT, accel_group);
  g_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (about), NULL);
  gtk_menu_append (GTK_MENU (submenu), menuitem);
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (helpmenu), submenu);

  //font,text
  hbox = gtk_hbox_new (FALSE, 0);
  button = gtk_font_button_new ();
  fontName = gtk_font_button_get_font_name (GTK_FONT_BUTTON (button));
  gtk_font_button_set_title (GTK_FONT_BUTTON (button), "字体");
  g_signal_connect (GTK_FONT_BUTTON (button), "font-set",	G_CALLBACK (setFont), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  entry = gtk_entry_new ();
  gtk_entry_set_text (GTK_ENTRY (entry), "文字内容");
  gtk_box_pack_start (GTK_BOX (hbox), entry, FALSE, TRUE, 5);

  //line width
  adjustment = gtk_adjustment_new (lw, 1, 50, 1, 1, 0);
  spin = gtk_spin_button_new (GTK_ADJUSTMENT (adjustment), 0.5, 0);
  g_signal_connect (GTK_OBJECT (spin), "value_changed",	G_CALLBACK (setLineWidth), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), spin, FALSE, TRUE, 0);
  
  //zoom
  button=gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_OUT,GTK_ICON_SIZE_BUTTON);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (zoomout), NULL);
  button=gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_IN,GTK_ICON_SIZE_BUTTON);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (zoomin), NULL);
  gtk_box_pack_start (GTK_BOX (mainbox), hbox, FALSE, TRUE, 0);
  button=gtk_button_new();
  gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
  image=gtk_image_new_from_stock(GTK_STOCK_ZOOM_100,GTK_ICON_SIZE_BUTTON);
  gtk_button_set_image(GTK_BUTTON(button),image);
  gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, TRUE, 0);
  g_signal_connect (button, "clicked", G_CALLBACK (zoom100), NULL);
  gtk_box_pack_start (GTK_BOX (mainbox), hbox, FALSE, TRUE, 0);

  //toolbar
  hbox = gtk_hbox_new (FALSE, 0);
  toolbar = gtk_toolbar_new ();
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar), GTK_TOOLBAR_ICONS);
  gtk_toolbar_set_orientation (GTK_TOOLBAR (toolbar),	GTK_ORIENTATION_VERTICAL);

  rectselect =gtk_radio_tool_button_new (NULL);
  image = gtk_image_new_from_file ("rectselect.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (rectselect), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (rectselect), "选取框");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), rectselect, -1);
  g_signal_connect (GTK_OBJECT (rectselect), "clicked",	GTK_SIGNAL_FUNC (drawSelect), NULL);
  
  pencil = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (rectselect));
  image = gtk_image_new_from_file ("pencil.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (pencil), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (pencil), "铅笔");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), pencil, -1);
  g_signal_connect (GTK_OBJECT (pencil), "clicked",	GTK_SIGNAL_FUNC (drawPoint), NULL);

  line = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("line.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (line), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (line), "直线");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), line, -1);
  g_signal_connect (GTK_OBJECT (line), "clicked", GTK_SIGNAL_FUNC (drawLine),	NULL);

  rect = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("rect.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (rect), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (rect), "方形");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), rect, -1);
  g_signal_connect (GTK_OBJECT (rect), "clicked", GTK_SIGNAL_FUNC (drawRect), NULL);

  ellipse = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("ellipse.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (ellipse), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (ellipse), "椭圆");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), ellipse, -1);
  g_signal_connect (GTK_OBJECT (ellipse), "clicked", GTK_SIGNAL_FUNC (drawArc), NULL);

  ttext = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("text.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (ttext), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (ttext), "文字");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), ttext, -1);
  fontDesc = pango_font_description_from_string (fontName);
  gdkfont = gdk_font_from_description (fontDesc);
  g_signal_connect (GTK_OBJECT (ttext), "clicked", GTK_SIGNAL_FUNC (drawText), NULL);

  eraser = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("eraser.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (eraser), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (eraser), "擦除");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), eraser, -1);
  g_signal_connect (GTK_OBJECT (eraser), "clicked",	GTK_SIGNAL_FUNC (drawErase), NULL);

  paint = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("paint.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (paint), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (paint), "填充");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), paint, -1);
  g_signal_connect (GTK_OBJECT (paint), "clicked",	GTK_SIGNAL_FUNC (drawPaint), NULL);

  move = gtk_radio_tool_button_new_from_widget (GTK_RADIO_TOOL_BUTTON (pencil));
  image = gtk_image_new_from_file ("move.png");
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (move), image);
  gtk_tool_item_set_tooltip_text (GTK_TOOL_ITEM (move), "移动");
  gtk_toolbar_insert (GTK_TOOLBAR (toolbar), move, -1);
   
  //scrolled=gtk_scrolled_window_new(NULL,NULL);
  //gtk_scrolled_window_set_policy((GtkScrolledWindow*)(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
  //gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled),toolbar);
  //gtk_box_pack_start(GTK_BOX(hbox),scrolled,FALSE,TRUE,0);
  vbox = gtk_vbox_new (FALSE, 0);

  //color dialog
  button = gtk_color_button_new ();
  gtk_color_button_set_title (GTK_COLOR_BUTTON (button), "前景颜色");
  g_signal_connect (button, "color-set", G_CALLBACK (setFGColor), NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, TRUE, 0);

  button=gtk_button_new();
  image = gtk_image_new_from_file ("clip.png");
  gtk_button_set_image(GTK_BUTTON(button),image);
  tooltips = gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltips, button, "裁剪","裁剪");
  g_signal_connect (button, "clicked",	GTK_SIGNAL_FUNC (clip), NULL);
  gtk_box_pack_start (GTK_BOX (vbox), button, FALSE, TRUE, 0);
  
  gtk_box_pack_start (GTK_BOX (vbox), toolbar, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), vbox, FALSE, TRUE, 0);

  //drawarea
  drawarea = gtk_drawing_area_new ();
  gtk_widget_set_size_request (drawarea, 600, 500);
  gtk_widget_add_events (drawarea, GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK | GDK_BUTTON1_MOTION_MASK);
  g_signal_connect (G_OBJECT (drawarea), "button_press_event", G_CALLBACK (draw_start), NULL);
  g_signal_connect (G_OBJECT (drawarea), "motion_notify_event",	G_CALLBACK (draw_move), NULL);
  g_signal_connect (G_OBJECT (drawarea), "button_release_event", G_CALLBACK (draw_end), NULL);
  g_signal_connect (G_OBJECT (drawarea), "expose_event", G_CALLBACK (expose_event_callback), NULL);
  g_signal_connect (G_OBJECT (drawarea), "configure_event",	G_CALLBACK (my_configure_event), NULL);

  scrolled = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy ((GtkScrolledWindow *) (scrolled),	GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  align = gtk_alignment_new (0, 0, 0, 0);
  gtk_container_add (GTK_CONTAINER (align), drawarea);
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), align);
  gtk_box_pack_start (GTK_BOX (hbox), scrolled, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (mainbox), hbox, TRUE, TRUE, 0);

  //statusbar
  hbox = gtk_hbox_new (FALSE, 0);
  statusbar1 = gtk_statusbar_new ();
  //gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(statusbar1),FALSE);
  context_id1 = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar1), "Statusbar1");
  gtk_statusbar_push (GTK_STATUSBAR (statusbar1), context_id1, "程序启动完毕");
  gtk_box_pack_start (GTK_BOX (hbox), statusbar1, TRUE, TRUE, 0);
  
  statusbar2 = gtk_statusbar_new ();
  context_id2 = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar2), "Statusbar2");
  gtk_box_pack_start (GTK_BOX (hbox), statusbar2, TRUE, TRUE, 0);
  
  statusbar3 = gtk_statusbar_new ();
  context_id3 = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar3), "Statusbar3");
  gtk_box_pack_start (GTK_BOX (hbox), statusbar3, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (mainbox), hbox, FALSE, TRUE, 0);
  gtk_widget_show_all (window);

  gc = gdk_gc_new (drawarea->window);
  gdk_gc_set_line_attributes (gc, lw, GDK_LINE_SOLID, GDK_CAP_ROUND, GDK_JOIN_ROUND);
  gdk_color_parse ("white", &erasecolor);
  gdk_color_parse ("white", &bgcolor);
  pixmap = gdk_pixmap_new (GDK_DRAWABLE (drawarea->window),	drawarea->allocation.width, drawarea->allocation.height, -1);
  gdk_draw_rectangle (pixmap, drawarea->style->white_gc, TRUE, 0, 0,	drawarea->allocation.width,	drawarea->allocation.height);

  gcDash = gdk_gc_new (drawarea->window);
  gdk_gc_set_line_attributes (gcDash, 1, GDK_LINE_ON_OFF_DASH, GDK_CAP_NOT_LAST,GDK_JOIN_MITER);
  drawSelect();  
  gtk_widget_queue_draw (drawarea);
  g_signal_connect(GTK_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), NULL);
  gtk_main ();
  return 0;
}
