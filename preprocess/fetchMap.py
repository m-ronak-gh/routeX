import osmnx as ox
import struct
import sys
import os

# Using structured queries for smaller, faster-to-process cities
CITIES = {
    "cambridge": {"city": "Cambridge", "country": "UK"},
    "pasadena": {"city": "Pasadena", "state": "California", "country": "USA"},
    "nara": {"city": "Nara", "country": "Japan"}
}

def fetch_and_export(city_key: str) -> None:
    if city_key not in CITIES:
        print(f"City '{city_key}' not found. Available cities: {list(CITIES.keys())}")
        sys.exit(1)
    
    place = CITIES[city_key]
    print(f"1. Fetching data for {place}...")

    # Fetch the graph for the specified city
    G = ox.graph_from_place(place, network_type='drive', simplify=True)

    nodes_gdf, edges_gdf = ox.graph_to_gdfs(G)
    print(f"2. Downloaded {len(nodes_gdf)} nodes and {len(edges_gdf)} edges.")

    # OSM node IDs are large 64-bit integers, here remaped to uint32
    node_list = list(nodes_gdf.index) # Extract list of node IDs
    id_map = {osm_id: i for i, osm_id in enumerate(node_list)} # Map OSM IDs to sequential integers
    n_nodes = len(node_list)

    # Count valid edges, both endpoints must be in id_map
    valid_edges = []
    for (u, v, _), row in edges_gdf.iterrows():
        if u in id_map and v in id_map:
            length = float(row.get("length", 1.0))  # Default to 1.0 if length is missing
            valid_edges.append((id_map[u], id_map[v], length))
    n_edges = len(valid_edges)

    print(f"3. Processed {n_edges} valid edges.")

    # Write to binary file
    # Header : uint32 (n_nodes), uint32 (n_edges)
    # Nodes  : n_nodes * (double lat, double lon) = n_nodes * 16 bytes
    # Edges  : n_edges * (uint32 u[from], uint32 v[to], double length[weight]) = n_edges * 16 bytes

    os.makedirs("data", exist_ok=True)
    output_file = f"data/{city_key}.bin"
    with open(output_file, "wb") as f:
        # Write header
        f.write(struct.pack("II", n_nodes, n_edges))

        # Write nodes
        for osm_id in node_list:
            row = nodes_gdf.loc[osm_id]
            lat = float(row["y"])
            lon = float(row["x"])
            f.write(struct.pack("dd", lat, lon))

        # Write edges
        for u, v, length in valid_edges:
            f.write(struct.pack("IId", u, v, length))
    
    file_size = os.path.getsize(output_file) // (1024) # Size in KB
    print(f"4. Exported data to '{output_file}' ({file_size:.2f} KB).")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <city_key>")
        print(f"Available cities: {list(CITIES.keys())}")
        sys.exit(1)
    
    city_key = sys.argv[1]
    fetch_and_export(city_key)