extern int far current_offset;
extern int far handle;
extern int far physical_page;
extern int far logical_page;
extern int far num_unalloc_pages;
extern int far page_frame_segment_address;
extern int far num_pages_requested;
extern char far no_more_ems_pages;
extern short far *start_of_page_frame_0;


extern void far get_board_status(void);
extern int far get_page_frame_address(void);
extern int far get_unalloc_page_count(void);
extern int far allocate_pages(int);
extern void far map_handle_page(int,int,int);
extern void far dealloc_pages_for_handle(int);


extern void far save_map_info(void);
extern void far restore_map_info(void);

extern void far exit_dealloc_pages(int);
