/*
 */

/*
 * Function: 	pages = pw_ems_init( num_pages )
 *
 * Purpose:	Verify EMS driver is present, and allocate memory
 *
 * Arguments:	int num_pages - The number of pages (size set by pw_page_size)
 *								to try to allocate
 *
 * Returns:	The number of pages allocated
 *				0 - No pages available, or no EMS driver present
 */
int	pw_ems_init( unsigned int );

/*
 * Function:	pw_ems_term()
 *
 * Purpose:	Release memory allocated in pw_ems_init()
 *
 * Arguments:	void
 *
 * Returns:	void
 *
 */
void	pw_ems_term( void );

/*
 * Function:	pw_ems_map( page, index )
 *
 * Purpose:	Map a page to an index
 *
 * Arguments:	int page  - The page you want (0 - (num_pages-1))
 *				int index - The EMS page index (0 - 3)
 *
 * Returns:	void
 *
 */
void	pw_ems_map( unsigned int, unsigned int );

/*
 * Function:	pw_ems_addr( index, page, pointer )
 *
 * Purpose:	Get the EMS address for an EMS index, page
 *
 * Arguments:	int index - The EMS page index
 *				it page - The page number
 *				far * far * short pointer - pointer to return full address in
 *
 * Returns:	void
 *
 */
void	pw_ems_addr( unsigned int, unsigned int, short far * far * );

/*
 * Function:	pw_ems_get( index, page, buffer )
 *
 * Purpose:	Copy data from a CEMM buffer into a DOS buffer
 *
 * Arguments:	int index - The EMS page index (0 - 3)
 *				int page - The page number
 *				short buffer[] - A buffer in DOS
 *
 * Returns:	void
 *
 */
void	pw_ems_get( unsigned int, unsigned int, short far * );

/*
 * Function:	pw_ems_put( index, page, buffer )
 *
 * Purpose:	Copy data from a DOS buffer into CEMM buffer
 *
 * Arguments:	int index - The EMS page index (0 - 3)
 *				int page - The page number
 *				short buffer[] - A 16K buffer in DOS
 *
 * Returns:	void
 *
 */
void	pw_ems_put( unsigned int, unsigned int, short far * );
