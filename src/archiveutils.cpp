#include "archiveutils.h"
#include <errno.h>

/**
	Helper function to extract a file using filesystem::path and std::string
*/
/*bool Archive::extract_s(fs::path from, std::string filename, fs::path to)
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
}*/

/**
	Extract from archive "from", the file "filename" into path "to" (filename has to be included here too)
*/
/*bool Archive::extract(const char* from, const char* filename, const char* to)
{
	struct archive* a;
	struct archive* ext;
	struct archive_entry* entry;
	int flags;
	int r;

	// Select which attributes we want to restore.
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
	if ((r = archive_read_open_filename(a, from, 10240)) != ARCHIVE_OK)
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
					return false;
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
}*/

bool Archive::extract_tar(fs::path from, std::string filename, fs::path to)
{
	std::string from_s = from.string(), to_s = to.string();

	try
	{
		Tar tarfile(from_s);
		if (!tarfile.extract(filename, to_s)) {
			fprintf(stderr, "Error while extracting %s. Not in the archive\n", filename.c_str());
			return false;
		}
		return true;
	}
	catch (const std::exception& exc)
	{
		fprintf(stderr, "%s\n", exc.what());
		return false;
	}
}

bool Archive::extract_zip(fs::path from, std::string filename, fs::path to)
{
	std::string from_s = from.string(), to_s = to.string();
	const char* from_c = from_s.c_str(), * to_c = to_s.c_str();
	int zerr = ZIP_ER_OK;
	zip_t* in_archive = zip_open(from_c, ZIP_RDONLY, &zerr);

	if (zerr != ZIP_ER_OK) {
		fprintf(stderr, "Error while opening %s\n", from_c);
		return false;
	}

	zip_file_t* target_file = zip_fopen(in_archive, filename.c_str(), 0);

	if (target_file == NULL) {
		fprintf(stderr, "Error while opening %s in %s. %s\n", filename.c_str(), from_c, zip_strerror(in_archive));
		zip_close(in_archive);
		return false;
	}

	FILE* out_f = fopen(to_c, "wb");

	char buffer[AR_BUFFER_SIZE];
	int sz = 0;
	while ((sz = zip_fread(target_file, buffer, AR_BUFFER_SIZE)) > 0) {
		fwrite(buffer, sizeof(char), sz, out_f);
	}

	fclose(out_f);

	zip_fclose(target_file);
	zip_close(in_archive);
	return true;
}