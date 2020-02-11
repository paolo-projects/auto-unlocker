#include "archiveutils.h"

/**
	Helper function to extract a file using filesystem::path and std::string
*/
bool Archive::extract_s(fs::path from, std::string filename, fs::path to)
{
	std::string from_s = from.string();
	std::string to_s = to.string();
	const char* from_c = from_s.c_str();
	const char* to_c = to_s.c_str();

	return extract(from_c, filename.c_str(), to_c);
}

int Archive::copy_data(struct archive* ar, struct archive* aw)
{
	int r;
	const void* buff;
	size_t size;
	la_int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			fprintf(stderr, "%s\n", archive_error_string(aw));
			return (r);
		}
	}
}

/**
	Extract from archive "from", the file "filename" into path "to" (filename has to be included here too)
*/
bool Archive::extract(const char* from, const char* filename, const char* to)
{
	struct archive* a;
	struct archive* ext;
	struct archive_entry* entry;
	int flags;
	int r;

	/* Select which attributes we want to restore. */
	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;

	a = archive_read_new();
	archive_read_support_format_all(a);
	archive_read_support_compression_all(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);
	if ((r = archive_read_open_filename(a, from, 10240)))
		return false;
	for (;;) {
		r = archive_read_next_header(a, &entry);

		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)
			logerr(archive_error_string(a));
		if (r < ARCHIVE_WARN)
			return false;

		const char* entry_path = archive_entry_pathname(entry);

		if (entry_path == NULL || strcmp(entry_path, filename) != 0)
			continue;
		else {
			archive_entry_set_pathname(entry, to);
			r = archive_write_header(ext, entry);
			if (r < ARCHIVE_OK)
				logerr(archive_error_string(ext));
			else if (archive_entry_size(entry) > 0) {
				r = copy_data(a, ext);
				if (r < ARCHIVE_OK)
					logerr(archive_error_string(ext));
				if (r < ARCHIVE_WARN)
					return false;
			}
			r = archive_write_finish_entry(ext);
			if (r < ARCHIVE_OK)
				logerr(archive_error_string(ext));
			if (r < ARCHIVE_WARN)
				return false;

			break;
		}
	}
	archive_read_close(a);
	archive_read_free(a);
	archive_write_close(ext);
	archive_write_free(ext);
	return true;
}
