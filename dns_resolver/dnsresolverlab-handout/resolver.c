#include<arpa/inet.h>
#include<netinet/in.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>

typedef unsigned int dns_rr_ttl;
typedef unsigned short dns_rr_type;
typedef unsigned short dns_rr_class;
typedef unsigned short dns_rdata_len;
typedef unsigned short dns_rr_count;
typedef unsigned short dns_query_id;
typedef unsigned short dns_flags;

typedef struct {
	char *name;
	dns_rr_type type;
	dns_rr_class class;
	dns_rr_ttl ttl;
	dns_rdata_len rdata_len;
	unsigned char *rdata;
} dns_rr;

struct dns_answer_entry;
struct dns_answer_entry {
	char *value;
	struct dns_answer_entry *next;
};
typedef struct dns_answer_entry dns_answer_entry;

void print_bytes(unsigned char *bytes, int byteslen) {
	int i, j, byteslen_adjusted;
	unsigned char c;

	if (byteslen % 8) {
		byteslen_adjusted = ((byteslen / 8) + 1) * 8;
	} else {
		byteslen_adjusted = byteslen;
	}
	for (i = 0; i < byteslen_adjusted + 1; i++) {
		if (!(i % 8)) {
			if (i > 0) {
				for (j = i - 8; j < i; j++) {
					if (j >= byteslen_adjusted) {
						printf("  ");
					} else if (j >= byteslen) {
						printf("  ");
					} else if (bytes[j] >= '!' && bytes[j] <= '~') {
						printf(" %c", bytes[j]);
					} else {
						printf(" .");
					}
				}
			}
			if (i < byteslen_adjusted) {
				printf("\n%02X: ", i);
			}
		} else if (!(i % 4)) {
			printf(" ");
		}
		if (i >= byteslen_adjusted) {
			continue;
		} else if (i >= byteslen) {
			printf("   ");
		} else {
			printf("%02X ", bytes[i]);
		}
	}
	printf("\n");
}

void canonicalize_name(char *name) {
	/*
	 * Canonicalize name in place.  Change all upper-case characters to
	 * lower case and remove the trailing dot if there is any.  If the name
	 * passed is a single dot, "." (representing the root zone), then it
	 * should stay the same.
	 *
	 * INPUT:  name: the domain name that should be canonicalized in place
	 */

	int namelen, i;

	// leave the root zone alone
	if (strcmp(name, ".") == 0) {
		return;
	}

	namelen = strlen(name);
	// remove the trailing dot, if any
	if (name[namelen - 1] == '.') {
		name[namelen - 1] = '\0';
	}

	// make all upper-case letters lower case
	for (i = 0; i < namelen; i++) {
		if (name[i] >= 'A' && name[i] <= 'Z') {
			name[i] += 32;
		}
	}
}

void get_top_dns(unsigned char *wire)
{
	//get top stuff before printing domain name
}


int name_ascii_to_wire(char *name, unsigned char *wire) {
	/*
	 * Convert a DNS name from string representation (dot-separated labels)
	 * to DNS wire format, using the provided byte array (wire).  Return
	 * the number of bytes used by the name in wire format.
	 *
	 * INPUT:  name: the string containing the domain name
	 * INPUT:  wire: a pointer to the array of bytes where the
	 *              wire-formatted name should be constructed
	 * OUTPUT: the length of the wire-formatted name.
	 */

	 char* temp;
	 int put_index = 0;
	 int arr_size = 1; //size of split array.
	 char* curr_name = NULL;
	 canonicalize_name(name);

	 temp = name;
	 while (*temp)
	 {
		 if ('.' == *temp)
		 {
			 arr_size++;
		 }
		 temp++;
	 }

	 //adds a place for character count of each part of the domain
	 char** tok_array = malloc(sizeof(char*)*arr_size);
	 if (tok_array)
	 {
		 int i = 0;
		 char* token = strtok(name, ".");
		 while(token)
		 {
			 *(tok_array+i) = malloc(strlen(token)*sizeof(char*));
			 strcpy(*(tok_array+i), token);
			 token = strtok(0, ".");
			 i++;
		 }
	 }

	 for (int i = 0; i < arr_size; i++)
	 {
		 curr_name = *(tok_array + i);
		 *(wire+put_index) = strlen(curr_name);
		 put_index++;
		 for (int j = 0; j < strlen(curr_name); j++)
		 {
			 *(wire+put_index) = curr_name[j];
			 put_index++;
			 //print_bytes(wire, put_index);
		 }
	 }
	 return put_index;
}

char *name_ascii_from_wire(unsigned char *wire, int *indexp) {
	/*
	 * Extract the wire-formatted DNS name at the offset specified by
	 * *indexp in the array of bytes provided (wire) and return its string
	 * representation (dot-separated labels) in a char array allocated for
	 * that purpose.  Update the value pointed to by indexp to the next
	 * value beyond the name.
	 *
	 * INPUT:  wire: a pointer to an array of bytes
	 * INPUT:  indexp, a pointer to the index in the wire where the
	 *              wire-formatted name begins
	 * OUTPUT: a string containing the string representation of the name,
	 *              allocated on the heap.
	 */
}

dns_rr rr_from_wire(unsigned char *wire, int *indexp, int query_only) {
	/*
	 * Extract the wire-formatted resource record at the offset specified by
	 * *indexp in the array of bytes provided (wire) and return a
	 * dns_rr (struct) populated with its contents. Update the value
	 * pointed to by indexp to the next value beyond the resource record.
	 *
	 * INPUT:  wire: a pointer to an array of bytes
	 * INPUT:  indexp: a pointer to the index in the wire where the
	 *              wire-formatted resource record begins
	 * INPUT:  query_only: a boolean value (1 or 0) which indicates whether
	 *              we are extracting a full resource record or only a
	 *              query (i.e., in the question section of the DNS
	 *              message).  In the case of the latter, the ttl,
	 *              rdata_len, and rdata are skipped.
	 * OUTPUT: the resource record (struct)
	 */
}


int rr_to_wire(dns_rr rr, unsigned char *wire, int query_only) {
	/*
	 * Convert a DNS resource record struct to DNS wire format, using the
	 * provided byte array (wire).  Return the number of bytes used by the
	 * name in wire format.
	 *
	 * INPUT:  rr: the dns_rr struct containing the rr record
	 * INPUT:  wire: a pointer to the array of bytes where the
	 *             wire-formatted resource record should be constructed
	 * INPUT:  query_only: a boolean value (1 or 0) which indicates whether
	 *              we are constructing a full resource record or only a
	 *              query (i.e., in the question section of the DNS
	 *              message).  In the case of the latter, the ttl,
	 *              rdata_len, and rdata are skipped.
	 * OUTPUT: the length of the wire-formatted resource record.
	 *
	 */
}

unsigned short create_dns_query(char *qname, dns_rr_type qtype, unsigned char *wire) {
	/*
	 * Create a wire-formatted DNS (query) message using the provided byte
	 * array (wire).  Create the header and question sections, including
	 * the qname and qtype.
	 *
	 * INPUT:  qname: the string containing the name to be queried
	 * INPUT:  qtype: the integer representation of type of the query (type A == 1)
	 * INPUT:  wire: the pointer to the array of bytes where the DNS wire
	 *               message should be constructed
	 * OUTPUT: the length of the DNS wire message
	 */

	 int body_len = 0; //where to start after header
	 unsigned int random1 = rand()%256;
	 unsigned int random2 = rand()%256;

	 unsigned char msg_header[] = {
		 random1, random2, 0x01, 0x00,
		 0x00, 0x01, 0x00, 0x00,
		 0x00, 0x00, 0x00, 0x00
	 };

	 unsigned char* msg_body = malloc(1024*sizeof(char*));

	 body_len = name_ascii_to_wire(qname, msg_body);
	 //add footer stuff

	 unsigned char msg_footer[] = {
		 0x00, 0x00, 0x01, 0x00, 0x01
	 };

	 memcpy(wire, msg_header, 12);
	 //print_bytes(wire, 12);
	 memcpy(wire+12, msg_body, body_len);
	 //print_bytes(wire, 12+body_len);
	 memcpy(wire+12+body_len, msg_footer, 5);
	 //print_bytes(wire, 12+body_len+5);
	 return 12+body_len+5;
}

dns_answer_entry *get_answer_address(char *qname, dns_rr_type qtype, unsigned char *wire) {
	/*
	 * Extract the IPv4 address from the answer section, following any
	 * aliases that might be found, and return the string representation of
	 * the IP address.  If no address is found, then return NULL.
	 *
	 * INPUT:  qname: the string containing the name that was queried
	 * INPUT:  qtype: the integer representation of type of the query (type A == 1)
	 * INPUT:  wire: the pointer to the array of bytes representing the DNS wire message
	 * OUTPUT: a linked list of dns_answer_entrys the value member of each
	 * reflecting either the name or IP address.  If

	 */
	 	int answer_sec_index = 2;
	 // set qname to the initial name queried
	 // 		(i.e., the query name in the question section)
		// 	for each resource record (RR) in the answer section:
		// 		if the owner name of RR matches qname and the type matches the qtype:
		// 		extract the address from the RR, convert it to a string, and add it
		// 		to the result list
		// 		else if the owner name of RR matches qname and the type is (5) CNAME:
		// 		the name is an alias; extract the canonical name from the RR rdata,
		// 		and set qname to that value, and add it to the result list
		// 		return NULL (no match was found)


}

int send_recv_message(unsigned char *request, int requestlen, unsigned char *response, char *server, unsigned short port) {
	/*
	 * Send a message (request) over UDP to a server (server) and port
	 * (port) and wait for a response, which is placed in another byte
	 * array (response).  Create a socket, "connect()" it to the
	 * appropriate destination, and then use send() and recv();
	 *
	 * INPUT:  request: a pointer to an array of bytes that should be sent
	 * INPUT:  requestlen: the length of request, in bytes.
	 * INPUT:  response: a pointer to an array of bytes in which the
	 *             response should be received
	 * OUTPUT: the size (bytes) of the response received
	 */
	 printf("sending on socket the request:\n");
	 print_bytes(request, requestlen);
	 struct sockaddr_in ser_addr;
	 response = malloc(1024*sizeof(char*));
 	 ser_addr.sin_family = AF_INET;
 	 ser_addr.sin_port = htons(port);
	 ser_addr.sin_addr.s_addr = inet_addr(server);

	 int sock = 0;
	 printf("socket\n");
	 if((sock = socket(AF_INET, SOCK_DGRAM, 0))<0)
	 {
		 printf("Error: socket creation\n");
		 return -1;
	 }

	 printf("connect\n");
	 if(connect(sock, (struct sockaddr*)&ser_addr, sizeof(ser_addr))<0)
	 {
		 printf("Error: connect on port - %d, ip - %d\n", ser_addr.sin_port, ser_addr.sin_addr.s_addr);
		 return -1;
	 }

	 printf("send\n");
	 if(send(sock, request, requestlen, 0) < 0)
	 {
		 printf("Error: sending request\n");
		 return -1;
	 }

	 printf("recv\n");
	 int resp_len = 0;
	 unsigned char * response_temp = malloc(1024*sizeof(char*));
	 resp_len = recv(sock, response_temp, 1024, 0);

	 print_bytes(response_temp, resp_len);
	 memcpy(response, response_temp, resp_len);
	 return resp_len;
}

dns_answer_entry *resolve(char *qname, char *server) {
	//start here
	dns_rr_type t = 1;
	char* namecpy = qname;
	unsigned char * wire = malloc(1024*sizeof(char*));
	unsigned char * resp_wire = malloc(1024*sizeof(char*));
	unsigned short length_of_query = 0;
	int resp_len = 0;

	//1. build DNS Query message
	length_of_query = create_dns_query(namecpy, t, wire);

	//2. Send DNS message
	resp_len = send_recv_message(wire, length_of_query, resp_wire, server, 53);
	print_bytes(resp_wire, resp_len);

	//3. extract answer from response
	get_answer_address(qname, 1, resp_wire);

	//5. Print that answer
}

int main(int argc, char *argv[]) {
	dns_answer_entry *ans;
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <domain name> <server>\n", argv[0]);
		exit(1);
	}
	ans = resolve(argv[1], argv[2]);
	while (ans != NULL) {
		printf("%s\n", ans->value);
		ans = ans->next;
	}
}