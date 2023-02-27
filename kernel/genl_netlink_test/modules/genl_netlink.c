// SPDX-License-Identifier: SimPL-2.0

#include <net/netlink.h>
#include <net/genetlink.h>
#include <linux/version.h>

#define LOGC(f, arg...) pr_notice(f"\n", ##arg)
#define TEST_GENL_MSG_FROM_KERNEL "Hello from kernel space!!!"

/* handler
 * message handling code goes here; return 0 on success, negative
 * values on failure
 */
static int doc_exmpl_echo(struct sk_buff *skb, struct genl_info *info);

/* netlink attributes */
enum {
	DOC_EXMPL_A_UNSPEC,
	DOC_EXMPL_A_MSG,
	__DOC_EXMPL_A_MAX,
};
#define DOC_EXMPL_A_MAX (__DOC_EXMPL_A_MAX - 1)

/* attribute policy */
static struct nla_policy doc_exmpl_genl_policy[DOC_EXMPL_A_MAX + 1] = {
	[DOC_EXMPL_A_MSG] = { .type = NLA_NUL_STRING }
};

/* commands 定义命令类型，用户空间以此来表明需要执行的命令 */
enum {
	DOC_EXMPL_C_UNSPEC,
	DOC_EXMPL_C_ECHO,
	__DOC_EXMPL_C_MAX,
};
#define DOC_EXMPL_C_MAX (__DOC_EXMPL_C_MAX - 1)
/* operation definition 将命令command echo和具体的handler对应起来 */
static struct genl_ops doc_exmpl_genl_ops_echo[] = {
	{
		.cmd = DOC_EXMPL_C_ECHO,
		.policy = doc_exmpl_genl_policy,
		.doit = doc_exmpl_echo,
	},
};

/* family definition */
static struct genl_family doc_exmpl_genl_family = {
	.name = "DOC_EXMPL",	//这里定义family的名称，user program需要根据这个名字来找到对应的family ID。
	.maxattr = DOC_EXMPL_A_MAX,
	.module = THIS_MODULE,
	.ops = doc_exmpl_genl_ops_echo,
	.n_ops = ARRAY_SIZE(doc_exmpl_genl_ops_echo),
};

static inline int genl_msg_prepare_usr_msg(u8 cmd, size_t size, pid_t pid, struct sk_buff **skbp)
{
	struct sk_buff *skb;

	/* create a new netlink msg */
	skb = genlmsg_new(size, GFP_KERNEL);
	if (skb == NULL)
		return -ENOMEM;

	/* Add a new netlink message to an skb */
	genlmsg_put(skb, pid, 0, &doc_exmpl_genl_family, 0, cmd);

	*skbp = skb;
	return 0;
}

static inline int genl_msg_mk_usr_msg(struct sk_buff *skb, int type, void *data, int len)
{
	int rc;

	/* add a netlink attribute to a socket buffer */
	rc = nla_put(skb, type, len, data);
	if (rc != 0)
		return rc;

	return 0;
}

/* genl_msg_send_to_user - 通过generic netlink发送数据到netlink		*/
/* @data: 发送数据缓存							*/
/* @len:数据长度 单位：byte						*/
/* @pid:发送到的客户端pid						*/
/*									*/
/* return:								*/
/*	0:	成功							*/
/*	-1:	失败							*/

int genl_msg_send_to_user(void *data, int len, pid_t pid)
{
	struct sk_buff *skb;
	size_t size;
	void *head;
	int rc;

	size = nla_total_size(len); /* total length of attribute including padding */

	rc = genl_msg_prepare_usr_msg(DOC_EXMPL_C_ECHO, size, pid, &skb);
	if (rc)
		return rc;

	rc = genl_msg_mk_usr_msg(skb, DOC_EXMPL_A_MSG, data, len);
	if (rc) {
		kfree_skb(skb);
		return rc;
	}

	head = genlmsg_data(nlmsg_data(nlmsg_hdr(skb)));

	genlmsg_end(skb, head);

	rc = genlmsg_unicast(&init_net, skb, pid);
	if (rc < 0)
		return rc;

	return 0;
}

//echo command handler, 命令处理函数，当接收到user program发出的命令后，这个函数会被内核调用
static int doc_exmpl_echo(struct sk_buff *skb, struct genl_info *info)
{
	/* message handling code goes here; return 0 on success, negative values on failure */
	struct nlmsghdr *nlhdr;
	struct genlmsghdr *genlhdr;
	struct nlattr *nlh;
	char *str;
	int ret;

	nlhdr = nlmsg_hdr(skb);
	genlhdr = nlmsg_data(nlhdr);
	nlh = genlmsg_data(genlhdr);
	str = nla_data(nlh);
	LOGC("%s get: %s\n", __func__, str);

	ret = genl_msg_send_to_user(TEST_GENL_MSG_FROM_KERNEL,
			strlen(TEST_GENL_MSG_FROM_KERNEL) + 1, nlhdr->nlmsg_pid);

	return ret;
}

int genlnetlink_init(void)
{
	int rc;

	rc = genl_register_family(&doc_exmpl_genl_family);
	if (rc != 0)
		goto err_out1;

	LOGC("genetlink_init OK %d", doc_exmpl_genl_family.id);
	return 0;

err_out1:
	genl_unregister_family(&doc_exmpl_genl_family);
	LOGC("Error occured while inserting generic netlink example module\n");
	return rc;
}

void genlnetlink_exit(void)
{
	LOGC("Generic Netlink Example Module unloaded.");
	genl_unregister_family(&doc_exmpl_genl_family);
}

module_init(genlnetlink_init);
module_exit(genlnetlink_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("genl netlink example");

