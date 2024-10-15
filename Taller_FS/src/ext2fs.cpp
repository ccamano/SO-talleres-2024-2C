#include "ext2fs.h"
#include "hdd.h"
#include "pentry.h"
#include "mbr.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdlib>
#include <unistd.h> //sleep() para debuggear
Ext2FS::Ext2FS(HDD & disk, unsigned char pnumber) : _hdd(disk), _partition_number(pnumber)
{
	assert(pnumber <= 3);

	// Load Superblock
	unsigned char * buffer = new unsigned char[2*SECTOR_SIZE];

	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Skip first two sectors & read 3rd and 4th sectors
	// Warning: We assume common sector size of 512bytes but it may not be true
	_hdd.read(pentry.start_lba()+2, buffer);
	_hdd.read(pentry.start_lba()+3, buffer+SECTOR_SIZE);

	// Warning: this only works in Little Endian arhitectures
	_superblock = (struct Ext2FSSuperblock *) buffer;

	// Load Block Group Table

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	// Compute amount of block groups in FS
	_block_groups = _superblock->blocks_count / _superblock->blocks_per_group;

	if((_superblock->blocks_count % _superblock->blocks_per_group) != 0)
	{
		_block_groups++;
	}
	std::cerr << "block_groups: " << _block_groups << std::endl;

	// Allocate enough memory to keep the table
	_bgd_table = new Ext2FSBlockGroupDescriptor[_block_groups];

	// Since the size of the table is not forced to be a factor of SECTOR_SIZE
	// we need to compute the amount of sectors we should read and then copy
	// one by one the descriptors into the table in the apropriate order.
	unsigned int size_of_bgd_table = sizeof(Ext2FSBlockGroupDescriptor) * _block_groups;
	unsigned int bgd_table_sectors = size_of_bgd_table / SECTOR_SIZE;
	if((size_of_bgd_table % SECTOR_SIZE) != 0)
	{
		bgd_table_sectors++;
	}
	std::cerr << "bgd_table_sectors: " << bgd_table_sectors << std::endl;

	buffer = new unsigned char[bgd_table_sectors * SECTOR_SIZE];
	for(unsigned int i = 0; i < bgd_table_sectors; i++)
	{
		std::cerr << "pentry.start_lba()+4+i: " << (unsigned int) (pentry.start_lba()+4+i) << " buffer+(SECTOR_SIZE*i): " << (unsigned char *) (buffer+(SECTOR_SIZE*i)) << std::endl;
		_hdd.read(pentry.start_lba()+4+i, buffer+(SECTOR_SIZE*i));
	}

	// Copy descriptors into table
	for(unsigned int i = 0; i < _block_groups; i++)
	{
		_bgd_table[i] = *((Ext2FSBlockGroupDescriptor *) (buffer+(i*sizeof(Ext2FSBlockGroupDescriptor))));
	}
	delete[] buffer;
}

unsigned int Ext2FS::blockgroup_for_inode(unsigned int inode)
{
	assert(inode > 0);
	return (inode - 1) / _superblock->inodes_per_group;
}

unsigned int Ext2FS::blockgroup_inode_index(unsigned int inode)
{
	assert(inode > 0);
	return (inode - 1) % _superblock->inodes_per_group;
}

// Free allocated memory
Ext2FS::~Ext2FS()
{
	delete[] _superblock;
	delete[] _bgd_table;
}

struct Ext2FSSuperblock * Ext2FS::superblock()
{
	return _superblock;
}

unsigned int Ext2FS::block_groups()
{
	return _block_groups;
}

struct Ext2FSBlockGroupDescriptor * Ext2FS::block_group(unsigned int index)
{
	assert(index < _block_groups);
	return & _bgd_table[index];
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSInode & inode)
{
	output << "mode: " << inode.mode << std::endl;
	output << "uid: " << inode.uid << std::endl;
	output << "size: " << inode.size << std::endl;
	output << "atime: " << inode.atime << std::endl;
	output << "ctime: " << inode.ctime << std::endl;
	output << "mtime: " << inode.mtime << std::endl;
	output << "dtime: " << inode.dtime << std::endl;
	output << "gid: " << inode.gid << std::endl;
	output << "links_count: " << inode.links_count << std::endl;
	output << "blocks: " << inode.blocks << std::endl;
	output << "flags: " << inode.flags << std::endl;
	//output << "os_dependant_1: " << inode.os_dependant_1 << std::endl;

	//output << std::hex;
	for(unsigned int i = 0; i < 15; i++)
		output << "block[" << i << "]: " << inode.block[i] << std::endl;
	//output << std:dec;

	output << "generation: " << inode.generation << std::endl;
	output << "file_acl: " << inode.file_acl << std::endl;
	output << "directory_acl: " << inode.directory_acl << std::endl;
	output << "faddr: " << inode.faddr << std::endl;
	//output << "os_dependant_2: " << inode.os_dependant_2 << std::endl[3];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSSuperblock & superblock)
{
	output << "inodes_count: " << superblock.inodes_count << std::endl;
	output << "blocks_count: " << superblock.blocks_count << std::endl;
	output << "reserved_blocks_count: " << superblock.reserved_blocks_count << std::endl;
	output << "free_blocks_count: " << superblock.free_blocks_count << std::endl;
	output << "free_inodes_count: " << superblock.free_inodes_count << std::endl;
	output << "first_data_block: " << superblock.first_data_block << std::endl;
	output << "log_block_size: " << superblock.log_block_size << std::endl;
	output << "log_fragment_size: " << superblock.log_fragment_size << std::endl;
	output << "blocks_per_group: " << superblock.blocks_per_group << std::endl;
	output << "fragments_per_group: " << superblock.fragments_per_group << std::endl;
	output << "inodes_per_group: " << superblock.inodes_per_group << std::endl;
	output << "mount_time: " << superblock.mount_time << std::endl;
	output << "write_time: " << superblock.write_time << std::endl;
	output << "mount_count: " << superblock.mount_count << std::endl;
	output << "max_mount_count: " << superblock.max_mount_count << std::endl;
	output << "magic_bytes: " << superblock.magic_bytes << std::endl;
	output << "state: " << superblock.state << std::endl;
	output << "errors: " << superblock.errors << std::endl;
	output << "minor_revision_level: " << superblock.minor_revision_level << std::endl;
	output << "lastcheck: " << superblock.lastcheck << std::endl;
	output << "checkinterval: " << superblock.checkinterval << std::endl;
	output << "creator_os: " << superblock.creator_os << std::endl;
	output << "revision_level: " << superblock.revision_level << std::endl;
	output << "default_reserved_userid: " << superblock.default_reserved_userid << std::endl;
	output << "default_reserved_groupid: " << superblock.default_reserved_groupid << std::endl;
	// -- EXT2_DYNAMIC_REV Specific --
	output << "first_inode: " << superblock.first_inode << std::endl;
	output << "inode_size: " << superblock.inode_size << std::endl;
	output << "block_group_number: " << superblock.block_group_number << std::endl;
	output << "feature_compatibility: " << superblock.feature_compatibility << std::endl;
	output << "feature_incompatibility: " << superblock.feature_incompatibility << std::endl;
	output << "readonly_feature_compatibility: " << superblock.readonly_feature_compatibility << std::endl;
	output << "uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.uuid[i];
	output << std::endl;
	output << "volume_name: ";
	for(int i = 0; i < 16; i++)
		output << superblock.volume_name[i];
	output << std::endl;
	output << "last_mounted: ";
	for(int i = 0; i < 64; i++)
		output << superblock.last_mounted[i];
	output << std::endl;
	output << "algo_bitmap: " << superblock.algo_bitmap << std::endl;
	// Performance hints
	output << "prealloc_blocks: " << (unsigned int) superblock.prealloc_blocks << std::endl;
	output << "prealloc_dir_blocks: " << (unsigned int) superblock.prealloc_dir_blocks << std::endl;
	//char alignment[2];
	// Journaling support
	output << "journal_uuid: ";
	for(int i = 0; i < 16; i++)
		output << superblock.journal_uuid[i];
	output << std::endl;
	output << "journal_inode: " << superblock.journal_inode << std::endl;
	output << "journal_device: " << superblock.journal_device << std::endl;
	output << "last_orphan: " << superblock.last_orphan << std::endl;
	// Directory indexing support
	output << "hash_seed: ";
	for(int i = 0; i < 4; i++)
		output << superblock.hash_seed[i];
	output << std::endl;
	output << "default_hash_version: " << (unsigned int) superblock.default_hash_version << std::endl;
	//char padding[3];
	// Other options
	output << "default_mount_options: " << superblock.default_mount_options << std::endl;
	output << "first_meta_bg: " << superblock.first_meta_bg << std::endl;
	//char unused[760];

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSBlockGroupDescriptor & bg_descriptor)
{
	output << "block_bitmap: " << bg_descriptor.block_bitmap << std::endl;
	output << "inode_bitmap: " << bg_descriptor.inode_bitmap << std::endl;
	output << "inode_table: " << bg_descriptor.inode_table << std::endl;
	output << "free_blocks_count: " << bg_descriptor.free_blocks_count << std::endl;
	output << "free_inodes_count: " << bg_descriptor.free_inodes_count << std::endl;
	output << "used_dirs_count: " << bg_descriptor.used_dirs_count << std::endl;

	return output;
}

std::ostream & operator<<(std::ostream & output, const struct Ext2FSDirEntry & dent)
{
	output << "inode: " << dent.inode << std::endl;
	output << "record_length: " << dent.record_length << std::endl;
	output << "name_length: " << (unsigned short) dent.name_length << std::endl;
	output << "file_type: " << (unsigned short) dent.file_type << std::endl;

	// NULL terminate dent name
	char * dentname = new char[dent.name_length + 1];
	strncpy(dentname, dent.name, dent.name_length);
	dentname[dent.name_length] = '\0';

	output << "name: " << dentname << std::endl;

	delete[] dentname;

	return output;
}

struct Ext2FSInode * Ext2FS::inode_for_path(const char * path)
{
	char * mypath = new char[strlen(path)+1];
	mypath[strlen(path)] = '\0';
	strncpy(mypath, path, strlen(path));

	char * pathtok = strtok(mypath, PATH_DELIM);
	struct Ext2FSInode * inode = NULL;

	while(pathtok != NULL)
	{
		struct Ext2FSInode * prev_inode = inode;
		// std::cerr << "pathtok: " << pathtok << std::endl;
		inode = get_file_inode_from_dir_inode(prev_inode, pathtok);
		pathtok = strtok(NULL, PATH_DELIM);

		delete prev_inode;
	}

	delete[] mypath;
	return inode;
}

unsigned int Ext2FS::blockaddr2sector(unsigned int block)
{
	// Get partition info
	const PartitionEntry & pentry = _hdd[_partition_number];

	// Compute block size by shifting the value 1024
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;

	return pentry.start_lba() + block * sectors_per_block;
}

/**
 * Warning: This method allocates memory that must be freed by the caller
 */
struct Ext2FSInode * Ext2FS::load_inode(unsigned int inode_number)
{
	//TODO: Ejercicio 1
	unsigned int blockgroup_num = blockgroup_for_inode(inode_number);
	struct Ext2FSBlockGroupDescriptor * descriptor_de_bloque = block_group(blockgroup_num);
	unsigned int index = blockgroup_inode_index(inode_number);
	
	unsigned int inodos_por_bloque = 1024 / (superblock())->inode_size;
	unsigned int bloque_de_inodo = index / inodos_por_bloque;  
	unsigned int indice_en_bloque = index % inodos_por_bloque;
	
	unsigned char* buffer = (unsigned char *) malloc(sizeof(Ext2FSInode) * inodos_por_bloque);
	read_block(descriptor_de_bloque->inode_table + bloque_de_inodo, buffer);
	
	Ext2FSInode * temp = &((Ext2FSInode *)buffer)[indice_en_bloque];

	return temp;
}

unsigned int Ext2FS::get_block_address(struct Ext2FSInode * inode, unsigned int block_number)
{

	//TODO: Ejercicio 2
	unsigned int block_size = 1024;
	unsigned int last_direct = 12;
	
	if (block_number < last_direct) {
	  //Bloque directo
	  return inode->block[block_number];
	} 
	
	unsigned int dirs_per_block = block_size/sizeof(unsigned int);
	unsigned int last_indirect = last_direct + dirs_per_block;
	if (block_number >= last_direct && block_number < last_indirect) {
	  //Indireccion simple
	  unsigned int indirect_block_index = block_number - last_direct; 
	  
	  unsigned int* indirect_buffer = (unsigned int*) malloc(sizeof(unsigned int*));
	  read_block(inode->block[12], (unsigned char *)indirect_buffer);

	  
	  unsigned int res =  indirect_buffer[indirect_block_index];
	  free(indirect_buffer);
	  
	  return res;

	  
    }
	
	unsigned int last_double_indirect = last_indirect + dirs_per_block*dirs_per_block;
	if(block_number >= last_indirect && block_number < last_double_indirect) {
        //Indireccion doble
		unsigned int nuevo_bloque = block_number - last_indirect;
		
		unsigned int double_indirect_block_index = nuevo_bloque / dirs_per_block;
		unsigned int indirect_block_index = nuevo_bloque % dirs_per_block;
		
		unsigned int* double_indirect_buffer = (unsigned int*) malloc(sizeof(unsigned int*));
		read_block(inode->block[13], (unsigned char *)double_indirect_buffer);
        
		unsigned int* indirect_buffer = (unsigned int*) malloc(sizeof(unsigned int*));
	    read_block(double_indirect_buffer[double_indirect_block_index], (unsigned char *)indirect_buffer);
		
		
		free(double_indirect_buffer);
		unsigned int res =  indirect_buffer[indirect_block_index];
		free(indirect_buffer);
		
		return res;

    }
	
	unsigned int last_triple_indirect = last_double_indirect + dirs_per_block*dirs_per_block*dirs_per_block;
	if(block_number >= last_double_indirect && block_number < last_triple_indirect){
		//Indireccion triple
		unsigned int nuevo_bloque = block_number - last_double_indirect;
		
		unsigned int triple_indirect_block_index = nuevo_bloque / (dirs_per_block*dirs_per_block);
		unsigned int nuevo_bloque_2 = nuevo_bloque % dirs_per_block*dirs_per_block;
		
		unsigned int double_indirect_block_index = nuevo_bloque_2 / dirs_per_block;
		unsigned int indirect_block_index = nuevo_bloque_2 % dirs_per_block;

		
		unsigned int* triple_indirect_buffer = (unsigned int*) malloc(sizeof(unsigned int*));
		read_block(inode->block[14], (unsigned char *)triple_indirect_buffer);

		unsigned int* double_indirect_buffer = (unsigned int*) malloc(sizeof(unsigned int*)); 
		read_block(triple_indirect_buffer[triple_indirect_block_index], (unsigned char *)double_indirect_buffer);
        
		unsigned int* indirect_buffer = (unsigned int*) malloc(sizeof(unsigned int*)); 
	    read_block(double_indirect_buffer[double_indirect_block_index], (unsigned char *)indirect_buffer);
		
		free(triple_indirect_buffer);
		free(double_indirect_buffer);
		unsigned int res =  indirect_buffer[indirect_block_index];
		free(indirect_buffer);
		
		return res;

	}

	//block_number es demasiado alto como para estar dentro de un inodo
	std::cout << "El número de bloque es demasiado grande para entrar en un inodo\n" << std::endl;

}

void Ext2FS::read_block(unsigned int block_address, unsigned char * buffer)
{
	unsigned int block_size = 1024 << _superblock->log_block_size;
	unsigned int sectors_per_block = block_size / SECTOR_SIZE;
	for(unsigned int i = 0; i < sectors_per_block; i++)
		_hdd.read(blockaddr2sector(block_address)+i, buffer+i*SECTOR_SIZE);
	}

struct Ext2FSInode * Ext2FS::get_file_inode_from_dir_inode(struct Ext2FSInode * from, const char * filename)
{
	if(from == NULL)
		from = load_inode(EXT2_RDIR_INODE_NUMBER);
	//std::cerr << *from << std::endl;
	assert(INODE_ISDIR(from));
    
	unsigned int block_size = 1024;
	
	//TODO: Ejercicio 3
    unsigned int cur_block_index = 0;
	
	
	unsigned int cur_block_addr = get_block_address(from,cur_block_index); 
	unsigned int next_block_addr = get_block_address(from,cur_block_index+1);
	
	
	bool foundFile = false;
	Ext2FSInode* res;

	unsigned int size = (((from->size)-1) / block_size) + 1; //cantidad de bloques maximos a explorar:  el ultimo byte del inodo del directorio (contando desde 0) sobre el tamaño de un bloque en bytes, +1 por el off by one. 
	
	std::cout << "Tamaño en bytes del inodo actual = " << from->size << std::endl;
	std::cout << "Tamaño en bloques del inodo actual = " << size << std::endl;

	Ext2FSDirEntry* block_buffer = (Ext2FSDirEntry*)malloc(sizeof(Ext2FSDirEntry*));
	bool skip_increment = false; //indica, para cada vez que nos pasamos del bloque, si necesitamos actualizar los indices de a uno o se dio un caso especial por un directorio entre dos o mas bloques
	unsigned int cur_block_byte = 0;
	while(!foundFile && cur_block_index < size){ 
        
		read_block(cur_block_addr,(unsigned char*)block_buffer);
		
		cur_block_byte = cur_block_byte % block_size;
		
		
		skip_increment = false;
		

		while(cur_block_byte < block_size && !foundFile){
			
            
			
			Ext2FSDirEntry* cur_dir_addr = block_buffer + cur_block_byte;
			unsigned short cur_length;
			std::string cur_name;
			unsigned int name_length;
			unsigned int cur_filetype;
			unsigned int inode_num;

			//Aprovecho que los structs dirEntry son __packed__
			if(cur_block_byte + sizeof(unsigned int) + sizeof(short) + 3 >= block_size){
				skip_increment = true;
				//el dir entry actual ocupa al menos dos bloques
				
				
				Ext2FSDirEntry* next_block_buffer = (Ext2FSDirEntry*)malloc(sizeof(Ext2FSDirEntry*));
				read_block(next_block_addr,(unsigned char*)next_block_buffer);
				
				
				char* name_start;
				std::string name_first_block = "";
				unsigned int name_start_byte;
				
				
				
				//Lo and behold, YandereDev code
				if(cur_block_byte+sizeof(unsigned int) >= block_size){
					//el numero de inodo queda partido entre los 2 bloques
					unsigned int inode_size_1 = block_size-cur_block_byte;
					unsigned int inode_size_2 = sizeof(unsigned int) - inode_size_1;
					
					//TODO: conseguir el numero de inodo entre los 2 bloques

					unsigned short length = *((unsigned short*)next_block_buffer+inode_size_2);
					name_length = *((unsigned int*)(next_block_buffer+inode_size_2 + sizeof(unsigned short)));
					name_start = (char*)(next_block_buffer+inode_size_2 + sizeof(unsigned short) + 2);
					cur_filetype = *((unsigned int*)(next_block_buffer+inode_size_2 + sizeof(unsigned short) + 1));
					name_start_byte = inode_size_2 + sizeof(unsigned short) + 2;
				}
				else if(cur_block_byte+sizeof(unsigned int) == block_size-1){
					//El bloque termina con el numero de inodo
					inode_num = cur_dir_addr->inode; 
					unsigned short length = *((unsigned short*)next_block_buffer);
					name_length = *((unsigned int*)(next_block_buffer+sizeof(unsigned short)));
					cur_filetype = *((unsigned int*)(next_block_buffer+sizeof(unsigned short)+1));
					name_start = (char*)(next_block_buffer + sizeof(unsigned short) + 2);
					name_start_byte = sizeof(unsigned short) + 2;
				}
				else if(cur_block_byte+sizeof(unsigned int) + sizeof(short) >= block_size){
					//el record_length queda partido entre los dos bloques
					inode_num = cur_dir_addr->inode; 
					unsigned int length_size_1 = 1;
					unsigned int length_size_2 = 1;

					
					name_length = *((unsigned int*)(next_block_buffer+length_size_2));
					cur_filetype = *((unsigned int*)(next_block_buffer+length_size_2+1));
					name_start = (char*)(next_block_buffer + length_size_2 + 2);
					name_start_byte = length_size_2 + 2;
				} 
				else if(cur_block_byte+sizeof(unsigned int)+sizeof(short) == block_size-1){
					//El bloque termina con el record length
					inode_num = cur_dir_addr->inode; 
					unsigned short length = cur_dir_addr->record_length; 
					name_length = *((unsigned int*)(next_block_buffer));
					cur_filetype = *((unsigned int*)(next_block_buffer+1));
					name_start = (char*)(next_block_buffer + 2);
					name_start_byte = 2;
				}
				else if(cur_block_byte+sizeof(unsigned int) + sizeof(short) + 1 == block_size-1){
					//El bloque termina con el name_length (como sizeof(char) es 1, no puede estar repartido entre bloques)
					inode_num = cur_dir_addr->inode; 
					unsigned short length = cur_dir_addr->record_length;
					name_length = cur_dir_addr->name_length;
					cur_filetype = *((unsigned int*)(next_block_buffer));
					name_start = (char*)(next_block_buffer + 1);
					name_start_byte = 1;
				}
				else if(cur_block_byte+sizeof(unsigned int) + sizeof(short) + 2 == block_size-1){
					//El bloque termina con el file_type (como sizeof(char) es 1, no puede estar repartido entre bloques)
					inode_num = cur_dir_addr->inode; 
					unsigned short length = cur_dir_addr->record_length;
					name_length = cur_dir_addr->name_length;
					cur_filetype = cur_dir_addr->file_type;
					name_start = (char*)(next_block_buffer);
					name_start_byte = 0;
				}
				else{
					//solo el nombre queda partido entre 2 archivos
					inode_num = cur_dir_addr->inode; 
					unsigned short length = cur_dir_addr->record_length;
					name_length = cur_dir_addr->name_length;
					cur_filetype = cur_dir_addr->file_type;
					name_start = (char*)(block_buffer+sizeof(unsigned int) + sizeof(short) + 2);
                    name_start_byte = 0;


					for(int i = 0; i + cur_block_byte+sizeof(unsigned int) + sizeof(short) + 2 < block_size; i++){
						name_first_block+=name_start[i]; 
					}
					name_first_block = std::string(name_first_block);
				}

				//-----Completar nombre de archivo--------///


				unsigned int unprocessed_name_length = name_length-name_first_block.size();
				if(name_start_byte+unprocessed_name_length < block_size){
					//Caso lindo: el resto del nombre se encuentra en un solo bloque
					std::string rest_of_name = "";
					for(int i = 0; i < unprocessed_name_length; i++){
						rest_of_name += name_start[i];
					}
					

					cur_name = (name_first_block + rest_of_name); 
					
					
					//Actualizacion de datos dado que entramos en siguiente bloque
					cur_block_addr = next_block_addr;
					cur_block_index++;  
					if(cur_block_index < size-1){
	  					next_block_addr = get_block_address(from,cur_block_index+1);
	   				}
					cur_block_byte = block_size + name_start_byte+unprocessed_name_length;
					free(next_block_buffer);
				}
				else{
					//Caso feo: el resto del nombre ocupa varios bloques.
					std::string rest_of_name = "";
					//Agrego el primer bloque;
					for(unsigned int i = 0; i < block_size-name_start_byte; i++){
						rest_of_name += name_start[i];
					}


					unsigned int extra_block_amount = name_length / block_size;
					unsigned int last_block_remainder = name_length % block_size;
					


					//agrego todos los bloques extras enteros menos el ultimo
					Ext2FSDirEntry* extra_block_buffer = (Ext2FSDirEntry*)malloc(sizeof(Ext2FSDirEntry*));
					for(unsigned int i = 0; i < extra_block_amount-1; i++){
						read_block(next_block_addr+i+1,(unsigned char*) extra_block_buffer);
						for(int j = 0; j < block_size; j++){
							char tmp = *((char*)(extra_block_buffer)+j);
							rest_of_name += tmp;
						}


					}

					read_block(next_block_addr+1+extra_block_amount,(unsigned char*) extra_block_buffer);
					for(int j = 0; j < last_block_remainder; j++){
							char tmp = *((char*)(extra_block_buffer)+j);
							rest_of_name += tmp;
						}

					free(extra_block_buffer);
					cur_name = (name_first_block + rest_of_name); 


					//Actualizo los datos en base a los bloques que salté
					cur_block_addr = get_block_address(from,cur_block_index+2+extra_block_amount) ;
					cur_block_index+= 2+extra_block_amount;  
					if(cur_block_index < size-1){
	  					next_block_addr = get_block_address(from,cur_block_index+1);
	   				}
					cur_block_byte = block_size + last_block_remainder;
					free(next_block_buffer);



				}


			}
			else{
				//Caso lindo: todo el struct esta en el mismo bloque
				std::cout << "cur_block_byte = " << cur_block_byte << ", cur_block_index = " << cur_block_index << std::endl;
				cur_length = cur_dir_addr->record_length;
				cur_filetype = cur_dir_addr->file_type;
				name_length = cur_dir_addr->name_length;
				cur_name = cur_dir_addr->name;
				inode_num = cur_dir_addr->inode;
				std::cout << *cur_dir_addr << std::endl;
				
				
				
			}
			cur_name = cur_name.substr(0,name_length);
			std::cout << inode_num << ", " << cur_length << ", " << cur_filetype << ", " << cur_name << std::endl;
			sleep(1);
			if(cur_name == std::string(filename)){
					foundFile = true;
					res = load_inode(cur_dir_addr->inode);
				}
			else if(cur_filetype == 2 && cur_name != std::string(".") && cur_name != std::string("..")){ //Encontramos un directorio: vamos un nivel mas abajo con un llamado recursivo.
				Ext2FSInode* rec_inode = load_inode(inode_num);
				Ext2FSInode* rec_res = get_file_inode_from_dir_inode(rec_inode, filename);
				if(rec_res != nullptr){
					foundFile = true;
					res = rec_res;
				}
				else{
				 cur_block_byte+= cur_length;
				}
			}
			else{
            	cur_block_byte+= cur_length;
			}

			

			
		}




        if(!skip_increment){

    		cur_block_addr = next_block_addr;
			cur_block_index++;  
			if(cur_block_index < size-1){
	  			next_block_addr = get_block_address(from,cur_block_index+1);
	   		}
		}
		

	}
	if(foundFile){
		return res;
	}
	else{
		Ext2FSInode* err = nullptr;
		return err;
	}

}

fd_t Ext2FS::get_free_fd()
{
	for(fd_t fd = 0; fd < EXT2_MAX_OPEN_FILES; fd++)
	{
		// Warning: This is inefficient
		if(!(_fd_status[fd/sizeof(unsigned int)] & (1 << (fd % sizeof(unsigned int)))))
			return fd;
	}
	return -1;
}

void Ext2FS::mark_fd_as_used(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] | (1 << (fd % sizeof(unsigned int))));
}

void Ext2FS::mark_fd_as_free(fd_t fd)
{

	_fd_status[fd/sizeof(unsigned int)] = (_fd_status[fd/sizeof(unsigned int)] ^ (1 << (fd % sizeof(unsigned int))));
}

fd_t Ext2FS::open(const char * path, const char * mode)
{
	fd_t new_fd = get_free_fd();
	if(new_fd < 0)
		return -1;
	mark_fd_as_used(new_fd);

	// We ignore mode
	struct Ext2FSInode * inode = inode_for_path(path);
	assert(inode != NULL);
	std::cerr << *inode << std::endl;

	if(inode == NULL)
		return -1;

	_open_files[new_fd] = *inode;
	delete inode;
	_seek_memory[new_fd] = 0;

	return new_fd;
}

int Ext2FS::read(fd_t filedesc, unsigned char * buffer, int size)
{
	int realsize = ((_seek_memory[filedesc] + size) >= _open_files[filedesc].size)?_open_files[filedesc].size-_seek_memory[filedesc]:size;
	if(realsize > 0)
	{
		int seek = _seek_memory[filedesc];
		unsigned int read = 0;

		// Compute block size by shifting the value 1024
		unsigned int block_size = 1024 << _superblock->log_block_size;
		unsigned int sectors_per_block = block_size / SECTOR_SIZE;

		unsigned int start_block = (seek / block_size);
		unsigned int end_block = ((seek + realsize - 1) / block_size);


		for(unsigned int block = start_block; block <= end_block; block++)
		{
			unsigned int block_address = get_block_address(&_open_files[filedesc], block);

			unsigned char block_buf[block_size];
			read_block(block_address, block_buf);

			do
			{
				buffer[read++] = block_buf[seek++ % block_size];
			} while(((seek % block_size) != 0) && (read < realsize));
		}
	}
	return realsize;
}

int Ext2FS::write(fd_t filedesc, const unsigned char * buffer, int size)
{
	return -1;
}

int Ext2FS::seek(fd_t filedesc, int offset)
{
	//std::cerr << "offset: " << offset << " size: " << _open_files[filedesc].size << std::endl;
	int position = offset;
	if(offset < 0)
		position = _open_files[filedesc].size + offset;
	if(position >= _open_files[filedesc].size)
		position = _open_files[filedesc].size - 1;
	_seek_memory[filedesc] = position;
	return position;
}

int Ext2FS::tell(fd_t filedesc)
{
	return _seek_memory[filedesc];
}

void Ext2FS::close(fd_t filedesc)
{
	mark_fd_as_free(filedesc);
}
