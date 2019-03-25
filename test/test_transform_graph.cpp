//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include <boost/test/unit_test.hpp>
#define protected public
#include <envire_core/graph/TransformGraph.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>
#include <string>
#include <envire_core/graph/GraphDrawing.hpp>

using namespace envire::core;
using namespace std;

class FrameProp 
{
public:
  string id;
  const string& getId() const {return id;}
  void setId(const string& _id) {id = _id;}
  
  const string toString() const
  {
      return id;
  }
  
  template<class Archive>
  void serialize(Archive &ar, const unsigned int version)
  {
    ar & id;
  }  
};

using Tfg = TransformGraph<FrameProp>;
using edge_descriptor = GraphTraits::edge_descriptor;
using vertex_descriptor = GraphTraits::vertex_descriptor;


void compareTranslation(const transformType& a, const transformType& b)
{
    BOOST_CHECK(a.transform.translation.x() == b.transform.translation.x());
    BOOST_CHECK(a.transform.translation.y() == b.transform.translation.y());
    BOOST_CHECK(a.transform.translation.z() == b.transform.translation.z());
  
}

void compareTransform(const transformType& a, const transformType& b)
{
    BOOST_CHECK(a.transform.translation.x() == b.transform.translation.x());
    BOOST_CHECK(a.transform.translation.y() == b.transform.translation.y());
    BOOST_CHECK(a.transform.translation.z() == b.transform.translation.z());
    BOOST_CHECK(a.transform.orientation.x() == b.transform.orientation.x());
    BOOST_CHECK(a.transform.orientation.y() == b.transform.orientation.y());
    BOOST_CHECK(a.transform.orientation.z() == b.transform.orientation.z()); 
    BOOST_CHECK(a.transform.orientation.w() == b.transform.orientation.w());
}


BOOST_AUTO_TEST_CASE(create_test)
{
    Tfg g;
}

BOOST_AUTO_TEST_CASE(get_transform_from_vertices_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    transformType tf;
    tf.transform.translation << 42, 42, 42;
    transformType invTf = tf.inverse();
    
    graph.add_edge(a, b, tf);
    vertex_descriptor aDesc = graph.getVertex(a);
    vertex_descriptor bDesc = graph.getVertex(b);
    
    transformType tf2 = graph.getTransform(aDesc, bDesc);
    compareTranslation(tf, tf2);
    
    transformType invtf2 = graph.getTransform(bDesc, aDesc);
    compareTranslation(invtf2, invTf);
    
}

BOOST_AUTO_TEST_CASE(get_transform_from_frameIds_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    transformType tf;
    tf.transform.translation << -13, 27, 0;
    transformType invTf = tf.inverse();
    
    graph.add_edge(a, b, tf);
    
    transformType tf2 = graph.getTransform(a, b);
    compareTranslation(tf, tf2);
    
    transformType invtf2 = graph.getTransform(b, a);
    compareTranslation(invtf2, invTf);  
}

BOOST_AUTO_TEST_CASE(get_transform_with_descriptor_without_edges_test)
{
    Tfg g;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    transformType tf;
    g.add_edge(a, b, tf);
    g.disconnectFrame(a);
    vertex_descriptor aDesc = g.vertex(a);
    vertex_descriptor bDesc = g.vertex(b);
    
    BOOST_CHECK_THROW(g.getTransform(aDesc, bDesc), UnknownTransformException);
}


BOOST_AUTO_TEST_CASE(update_transform_with_descriptors_on_disconnected_graph_test)
{
    Tfg g;
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    transformType tf;
    g.add_edge(a, b, tf);
    g.disconnectFrame(a);
    vertex_descriptor aDesc = g.vertex(a);
    vertex_descriptor bDesc = g.vertex(b);
    //test on totally disconnected graph
    BOOST_CHECK_THROW(g.updateTransform(aDesc, bDesc, tf), UnknownEdgeException);
    
    //test on partially disconnected graph
    FrameId c = "frame_c";
    g.add_edge(a, c, tf);
    BOOST_CHECK_THROW(g.updateTransform(aDesc, bDesc, tf), UnknownEdgeException);
    
}

BOOST_AUTO_TEST_CASE(simple_add_get_transform_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Tfg graph;
    transformType tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(graph.add_edge(a, b, tf));
    BOOST_CHECK(graph.num_edges() == 2);
    BOOST_CHECK(graph.num_vertices() == 2);
    transformType readTf = graph.getTransform(a, b);
    compareTransform(readTf, tf);
    transformType invTf = tf;
    invTf.setTransform(tf.transform.inverse());
    transformType readTfInv = graph.getTransform(b, a);
    compareTransform(readTfInv, invTf);
}


BOOST_AUTO_TEST_CASE(complex_add_get_transform_test)
{

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    Tfg graph;
    transformType tf;
    tf.transform.translation << 42, 21, -42;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitX());
    BOOST_CHECK_NO_THROW(graph.addTransform(a, b, tf));
    BOOST_CHECK_NO_THROW(graph.addTransform(b, c, tf));
    BOOST_CHECK_NO_THROW(graph.addTransform(a, d, tf));
    BOOST_CHECK(graph.num_edges() == 6);
    BOOST_CHECK(graph.num_vertices() == 4);

    /* a -> c **/
    transformType readTf;
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(a, c));
    compareTransform(readTf, tf*tf);

    /* c -> a **/
    transformType invTf;
    invTf.setTransform(tf.transform.inverse() * tf.transform.inverse());
    transformType readTfInv;
    BOOST_CHECK_NO_THROW(readTfInv = graph.getTransform(c, a));
    compareTransform(readTfInv, invTf);

    /* a -> d **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(a, d));
    compareTransform(readTf, tf);

    /* c -> d **/
    transformType complexTf;
    complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(c, d));
    compareTransform(readTf, complexTf);

    /* d -> c **/
    complexTf.setTransform(tf.transform.inverse()*tf.transform*tf.transform);
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(d, c));
    compareTransform(readTf, complexTf);

    //Close a cycle with an extra frame.
    //In practice it should not happen in envire
    FrameId e = "frame_e";
    BOOST_CHECK_NO_THROW(graph.addTransform(d, e, tf));
    BOOST_CHECK_NO_THROW(graph.addTransform(e, c, tf));

    /* d -> c **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(d, c));
    compareTransform(readTf, tf*tf);

    /* a-> e **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(a, e));
    compareTransform(readTf, tf*tf);

    FrameId f = "frame_f";
    BOOST_CHECK_NO_THROW(graph.addTransform(d, f, tf));

    /* c -> f **/
    BOOST_CHECK_NO_THROW(readTf = graph.getTransform(c, f));
    complexTf.setTransform(tf.transform.inverse()*tf.transform.inverse()*tf.transform);
    compareTransform(readTf, complexTf);

}

BOOST_AUTO_TEST_CASE(add_transform_exception_test)
{
    FrameId a = "frame_a";
    FrameId b = "frame_b";
    Tfg graph;
    transformType tf;
    BOOST_CHECK_NO_THROW(graph.addTransform(a, b, tf));
    BOOST_CHECK_THROW(graph.addTransform(b, a, tf), EdgeAlreadyExistsException);
}

BOOST_AUTO_TEST_CASE(get_transform_exception_test)
{
    FrameId a = "frame_a";
    FrameId c = "frame_c";
    Tfg graph;
    transformType tf;
    BOOST_CHECK_THROW(graph.getTransform(a, c), UnknownFrameException);
}


BOOST_AUTO_TEST_CASE(get_transform_between_unconnected_trees)
{
    Tfg graph;
    FrameId a("a");
    FrameId b("b");
    FrameId c("c");
    FrameId d("d");
    transformType tf;

    graph.addTransform(a, b, tf);
    graph.addTransform(c, d, tf);
    BOOST_CHECK_THROW(graph.getTransform(a, c), UnknownTransformException);
}

BOOST_AUTO_TEST_CASE(get_transform_using_a_tree)
{

    Tfg graph;
    /*       a
     *      / \
     *     c   b
     *   /  \
     *  d   e
     *    /  \
     *   f   g
     */

    FrameId a = "frame_a";
    FrameId b = "frame_b";
    FrameId c = "frame_c";
    FrameId d = "frame_d";
    FrameId e = "frame_e";
    FrameId f = "frame_f";
    FrameId g = "frame_g";

    transformType tf;
    tf.transform.translation << 1, 2, 1;
    tf.transform.orientation = base::AngleAxisd(0.25, base::Vector3d::UnitZ());

    graph.addTransform(a, b, tf);
    graph.addTransform(a, c, tf);
    graph.addTransform(c, d, tf);
    graph.addTransform(c, e, tf);
    graph.addTransform(e, f, tf);
    graph.addTransform(e, g, tf);

    //use a as root
    TreeView view = graph.getTree(graph.getVertex(a));
    BOOST_CHECK(view.root == graph.getVertex(a));

    transformType tree_tf_a_f = graph.getTransform(a, f, view);
    transformType graph_tf_a_f = graph.getTransform(a, f);

    /** Compare transform cannot be used due to round-off errors **/
    BOOST_CHECK(tree_tf_a_f.transform.translation.isApprox(graph_tf_a_f.transform.translation));
    BOOST_CHECK(tree_tf_a_f.transform.orientation.isApprox(graph_tf_a_f.transform.orientation));

    transformType tree_tf_d_g = graph.getTransform(d, g, view);
    transformType graph_tf_d_g = graph.getTransform(d, g);


    /** Compare transform cannot be used due to round-off errors **/
    BOOST_CHECK(tree_tf_d_g.transform.translation.isApprox(graph_tf_d_g.transform.translation));
    BOOST_CHECK(tree_tf_d_g.transform.orientation.isApprox(graph_tf_d_g.transform.orientation));

    transformType tree_tf_f_g = graph.getTransform(f, g, view);
    transformType graph_tf_f_g = graph.getTransform(f, g);


    /** Compare transform cannot be used due to round-off errors **/
    BOOST_CHECK(tree_tf_f_g.transform.translation.isMuchSmallerThan(0.01));
    BOOST_CHECK(graph_tf_f_g.transform.translation.isApprox(Eigen::Vector3d::Zero()));
    BOOST_CHECK(tree_tf_f_g.transform.orientation.isApprox(graph_tf_f_g.transform.orientation));

    /** Identity **/
    transformType tree_tf_f_f = graph.getTransform(f, f, view);
    BOOST_CHECK(tree_tf_f_f.transform.translation.isApprox(Eigen::Vector3d::Zero()));
    BOOST_CHECK(tree_tf_f_f.transform.orientation.isApprox(Eigen::Quaterniond::Identity()));
}


BOOST_AUTO_TEST_CASE(get_transform_with_descriptor_between_unconnected_frames_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    graph.addFrame(a);
    graph.addFrame(b);
    
    vertex_descriptor aDesc = graph.getVertex(a);
    vertex_descriptor bDesc = graph.getVertex(b);
    
    BOOST_CHECK_THROW(graph.getTransform(aDesc, bDesc), UnknownTransformException);
    
}

BOOST_AUTO_TEST_CASE(get_transform_from_edge_test)
{
    Tfg graph;
    FrameId a("A");
    FrameId b("B");
    transformType tf;
    tf.transform.translation << 42, 42, 42;
    
    graph.addTransform(a, b, tf);
    edge_descriptor edge = graph.getEdge(a, b);
    transformType tf2 = graph.getTransform(edge);
    BOOST_CHECK(tf.transform.translation.x() == tf2.transform.translation.x());
    BOOST_CHECK(tf.transform.translation.y() == tf2.transform.translation.y());
    BOOST_CHECK(tf.transform.translation.z() == tf2.transform.translation.z());
}


BOOST_AUTO_TEST_CASE(transform_graph_graphviz_test)
{
    Tfg graph;
    
    
    FrameId a("Captain Kathryn Janeway");
    FrameId b("Commander Chakotay");
    FrameId c("Lieutenant Commander Tuvok");
    FrameId d("Lieutenant Tom Paris");
    FrameId e("Lieutenant B’Elanna Torres");
    FrameId f("Medizinisch-holografisches Notfallprogramm");
    
    transformType ab;
    ab.transform.translation << 42, 42, 42;
    transformType ac;
    ac.transform.translation << 13, 0, 21;
    transformType ad;
    ad.transform.translation << -42, 2, 31;
    
    transformType de;
    de.transform.translation << 0.5, 0, 99;
    transformType ce;
    ce.transform.translation << 0.00001, 0, 11;
    transformType ef;
    ef.transform.translation << -0.000234, 0, 81;
    
    graph.addTransform(a,b, ab);
    graph.addTransform(a, c, ac);
    graph.addTransform(a, d, ad);
    graph.addTransform(d, e, de);
    graph.addTransform(c, e, ce);
    graph.addTransform(e, f, ef);
    
    GraphDrawing::write(graph, "transformgraph_graphviz_test.dot");

}

BOOST_AUTO_TEST_CASE(transform_graph_serialization_test)
{
    FrameId a = "AA";
    FrameId b = "BBB";
    FrameId c = "CCCC";
    Tfg graph;
    transformType ab;
    transformType bc;
    
    ab.transform.translation << 1, 2, 3;
    ab.transform.orientation.coeffs() << 0, 1, 2, 3;
    ab.time = base::Time::now();
    bc.transform.translation << 4, 5, 6;
    bc.transform.orientation.coeffs() << -0, 2, 4, 6;
  
    graph.addTransform(a, b, ab);
    graph.addTransform(b,c, bc);
    
    std::stringstream stream;
    boost::archive::binary_oarchive oa(stream);
    oa << graph;
    boost::archive::binary_iarchive ia(stream);
    Tfg graph2;
    ia >> graph2;   
    
    BOOST_CHECK(graph2.num_edges() == graph.num_edges());
    BOOST_CHECK(graph2.num_vertices() == graph.num_vertices());
    //check if vertices exist
    BOOST_CHECK_NO_THROW(graph2.getVertex(a));
    BOOST_CHECK_NO_THROW(graph2.getVertex(b));
    BOOST_CHECK_NO_THROW(graph2.getVertex(c));
    //check if edges exist
    BOOST_CHECK_NO_THROW(graph2.getEdge(a, b));
    BOOST_CHECK_NO_THROW(graph2.getEdge(b, a));
    BOOST_CHECK_NO_THROW(graph2.getEdge(b, c));
    BOOST_CHECK_NO_THROW(graph2.getEdge(c, b));
    //check if edge property was loaded correctly
    transformType ab2 = graph.getTransform(a, b);
    transformType bc2 = graph.getTransform(b, c);
    compareTransform(ab, ab2);
    compareTransform(bc, bc2);
}


BOOST_AUTO_TEST_CASE(remove_transform_test)
{
    FrameId a = "aaa";
    FrameId b = "bbb";
    FrameId c = "ccc";
    Tfg g;
    transformType ab;
    transformType bc;
    
    g.addTransform(a, b, ab);
    g.addTransform(b, c, bc);
    
    g.removeTransform(a, b);
    BOOST_CHECK_THROW(g.getTransform(a, b), UnknownTransformException);
    BOOST_CHECK_THROW(g.getTransform(b, a), UnknownTransformException);
    BOOST_CHECK_NO_THROW(g.getTransform(b, c));
    
    vertex_descriptor bVertex = g.getVertex(b);
    vertex_descriptor cVertex = g.getVertex(c);
    g.removeTransform(cVertex, bVertex);
    BOOST_CHECK_THROW(g.getTransform(bVertex, cVertex), UnknownTransformException);
    BOOST_CHECK_THROW(g.getTransform(cVertex, bVertex), UnknownTransformException);

}

BOOST_AUTO_TEST_CASE(get_path_transform_test)
{
  /*Graph:       A
   *           /   \
   *           |    B
   *           |    |
   *           D -- C
   *           |    |
   *           E    G
   *           |    |
   *           F -- H
   *           |
   *           I
   */
  
      
    Tfg graph;
    
    transformType tf;
    tf.transform.translation << 1,2,3;
    tf.transform.orientation = Eigen::Quaterniond(1,2,3,4);
    graph.addTransform("A", "B", tf);
    tf.transform.translation << 0,-1,42;
    tf.transform.orientation = Eigen::Quaterniond(1,0,0,13);
    graph.addTransform("A", "D", tf);
    tf.transform.translation << 0,-1,13;
    tf.transform.orientation = Eigen::Quaterniond(1,0,0,13);
    graph.addTransform("D", "E", tf);
    tf.transform.translation << 0,-1,0;
    tf.transform.orientation = Eigen::Quaterniond(1,0,0,13);
    graph.addTransform("E", "F", tf);
    tf.transform.translation << 12,-1,42;
    tf.transform.orientation = Eigen::Quaterniond(1,0,0,13);
    graph.addTransform("F", "I", tf);
    tf.transform.translation << 17,12,14;
    tf.transform.orientation = Eigen::Quaterniond(1,2,0,13);
    graph.addTransform("B", "C", tf);
    tf.transform.translation << 0,-1,42;
    tf.transform.orientation = Eigen::Quaterniond(0.3,0,0,0.5);
    graph.addTransform("C", "G", tf);
    tf.transform.translation << 0,-1,42;
    tf.transform.orientation = Eigen::Quaterniond(0.5,0.4,0.10,0.2);
    graph.addTransform("G", "H", tf);
    
    transformType tfAb = graph.getTransform("A", "B");
    transformType tfAi = graph.getTransform("A", "I");
    transformType tfDh = graph.getTransform("D", "H");
    transformType tfIb = graph.getTransform("I", "B");
    
    std::shared_ptr<Path> ab = graph.getPath("A", "B", false);
    std::shared_ptr<Path> ai = graph.getPath("A", "I", false);
    std::shared_ptr<Path> dh = graph.getPath("D", "H", false);
    std::shared_ptr<Path> ib = graph.getPath("I", "B", false);   
    
    transformType tfPathAb = graph.getTransform(ab);
    transformType tfPathAi = graph.getTransform(ai);
    transformType tfPathDh = graph.getTransform(dh);
    transformType tfPathIb = graph.getTransform(ib);
    
    compareTransform(tfAb, tfPathAb);
    compareTransform(tfAi, tfPathAi);
    compareTransform(tfDh, tfPathDh);
    compareTransform(tfIb, tfPathIb);
}


BOOST_AUTO_TEST_CASE(get_path_transform_dirty_test)
{
 /** Initial grap:
  *   A -> B -> C
  *    \
  *     D -> E
  * Graph after change:
  *   A -> D -> E -> C -> B
 */
      
    Tfg graph;
    
    transformType tf;
    tf.transform.translation << 0,3,0;
    tf.transform.orientation = Eigen::Quaterniond(1,2,3,4);
    graph.addTransform("A", "B", tf);
    tf.transform.translation << 0,-1,42;
    tf.transform.orientation = Eigen::Quaterniond(1,0,0,13);
    graph.addTransform("B", "C", tf);
    tf.transform.translation << -5,1,13;
    tf.transform.orientation = Eigen::Quaterniond(1,2,0,13);
    graph.addTransform("A", "D", tf);
    tf.transform.translation << -5,12,10;
    tf.transform.orientation = Eigen::Quaterniond(1,-1,0,13);
    graph.addTransform("D", "E", tf);

    
    std::shared_ptr<Path> path = graph.getPath("A", "B", true);
    
    graph.remove_edge("A", "B");
    BOOST_CHECK(path->isDirty());
    graph.addTransform("E", "C", tf);
    compareTransform(graph.getTransform(path), graph.getTransform("A", "B"));

}

BOOST_AUTO_TEST_CASE(get_path_transform_dirty_exception_test)
{
    Tfg graph;
    
    transformType tf;
    tf.transform.translation << 0,3,0;
    tf.transform.orientation = Eigen::Quaterniond(1,2,3,4);
    graph.addTransform("A", "B", tf);
    tf.transform.translation << 0,-1,42;
    tf.transform.orientation = Eigen::Quaterniond(1,0,0,13);
    graph.addTransform("B", "C", tf);
    tf.transform.translation << -5,1,13;
    tf.transform.orientation = Eigen::Quaterniond(1,2,0,13);

    
    std::shared_ptr<Path> path = graph.getPath("A", "C", true);
    
    graph.remove_edge("A", "B");
    BOOST_CHECK_THROW(graph.getTransform(path), InvalidPathException);
}

