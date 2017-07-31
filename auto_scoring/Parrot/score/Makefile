
CFLAGS = -g -fPIC -c

LDFLAGS = -rdynamic -lm -ldl -lpthread

SOFLAGS = -shared -Wl,--no-undefined -Wl,--as-needed -Wl,-soname=$@

INCLUDE = -I.



all: compute-mlp-feats

compute-mlp-feats: liblink_list.so obtain_mlp_fea.o
	gcc $(LDFLAGS) -o $@ $^ -Wl,-rpath=/work7/tangzy/github/auto_scoring_20170726/auto_scoring/compare/score

liblink_list.so: link_list.o
	gcc $(LDFLAGS) $(SOFLAGS) -o $@ $^ 

%.o: %.c
	gcc $(CFLAGS) $(INCLUDE) -o $@ $<

clean:
	-rm compute-mlp-feats *.o liblink_list.so

