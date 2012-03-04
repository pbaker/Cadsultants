
#define NUM_EMS_PAGES 60



extern char save_map_handle_page;
extern int  current_offset;
extern int  handle;
extern int  physical_page;
extern int  logical_page;
extern int  num_unalloc_pages;
extern int  page_frame_segment_address;
extern int  num_pages_requested;
extern short  *physical_pages_pointer;
extern int  current_logical_page;
extern int  current_physical_page;

extern char my_ems_pages[NUM_EMS_PAGES][16384];

extern void  get_board_status();
extern int  get_page_frame_address();
extern int  get_unalloc_page_count();
extern int  allocate_pages();
extern void  map_handle_page();
extern void  dealloc_pages_for_handle();


extern void  megatek_map_handle();  /*new (for d135 only)*/
extern void  save_map_info();
extern void  restore_map_info();


/* NEW for Single Screen D135 ONLY */
extern void  blt_map_handle();  

